//
// Created by Alexander Tampier on 20.11.18.
//

#ifndef SERVER_LDAP_H
#define SERVER_LDAP_H

#define LDAP_HOST "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define ANONYMOUS_USER "" // uid=if17b064,ou=People,dc=technikum-wien,dc=at
#define ANONYMOUS_PW "" // plain text password

#include <string>
#include <ldap.h>
#include <sstream>

bool my_login(std::string parameter_user, std::string parameter_password);

/**
 * Login to LDAP using given user and pw and return if successful.
 */
bool my_login(std::string parameter_user, std::string parameter_password) {
    // bind anonymous
    // structure for returning a sequence of octet strings + length
    BerValue *servercredp;
    BerValue cred;
    cred.bv_val = (char *) ANONYMOUS_PW;
    cred.bv_len = strlen(ANONYMOUS_PW);

    // search parameter_user and get distinguished name if parameter_user was found

    char *attribs[3];        /* attribute array for search */

    attribs[0] = strdup("uid");        /* return uid and cn of entries */
    attribs[1] = strdup("cn");
    attribs[2] = NULL;        /* array must be NULL terminated */

    const char *filter = ((std::string) ("(uid=" + parameter_user + ")")).c_str();
    LDAPMessage *result; // LDAP result handle
    std::string userDN = "";

    bool loginSuccess = false;

    int resultCount = 0;

    // setup connection
    // Setup LDAP connection and return resource handle.
    LDAP *ld; // LDAP resource handle
    int ldapversion = LDAP_VERSION3;
    int rc = 0;

    if ((ld = ldap_init(LDAP_HOST, LDAP_PORT)) == NULL) {
        printf("ldap_init failed\n");
        fflush(stdout);
        return false;
    }

    printf("connected to LDAP server %s on port %d\n", LDAP_HOST, LDAP_PORT);
    fflush(stdout);

    // ldap always listening in the lowest protocol version
    if ((rc = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ldapversion)) != LDAP_SUCCESS) {
        printf("ldap_set_option(PROTOCOL_VERSION): %s", ldap_err2string(rc));
        fflush(stdout);
        ldap_unbind_ext_s(ld, NULL, NULL);
        return false;
    }

    if ((rc = ldap_start_tls_s(ld, NULL, NULL)) != LDAP_SUCCESS) {
        printf("ldap_start_tls_s(): %s", ldap_err2string(rc));
        fflush(stdout);
        ldap_unbind_ext_s(ld, NULL, NULL);
        return false;
    }

    // Anonymous LDAP bind.
    if ((rc = ldap_sasl_bind_s(ld, ANONYMOUS_USER, LDAP_SASL_SIMPLE, &cred, NULL, NULL, &servercredp)) !=
        LDAP_SUCCESS) {
        fprintf(stderr, "LDAP error: %s\n", ldap_err2string(rc));
        fflush(stdout);
        ldap_unbind_ext_s(ld, NULL, NULL);
        return false;
    } else {
        printf("bind successful\n");
        fflush(stdout);
    }


    // search for parameter_user
    rc = ldap_search_s(ld, SEARCHBASE, SCOPE, filter, attribs, 0, &result);

    if (rc != LDAP_SUCCESS) {
        fprintf(stderr, "LDAP error: %s\n", ldap_err2string(rc));
        fflush(stdout);
        ldap_unbind_ext_s(ld, NULL, NULL);
        return false;
    } else {
        printf("parameter_user found successful\n");
        fflush(stdout);
    }

    // count results
    resultCount = ldap_count_entries(ld, result);

    // if parameter_user is found and unique
    if (resultCount == 1) {
        // load entry
        LDAPMessage *e = ldap_first_entry(ld, result);

        // set distinguished name
        userDN = ldap_get_dn(ld, e);

        // check cn attribute
        BerElement *ber; // array of attributes
        char *attribute = ldap_first_attribute(ld, e, &ber);
        BerValue **vals = ldap_get_values_len(ld, e, attribute);

        if (vals != NULL) {
            std::string user_common_name = "";

            // loop values
            for (int i = 0; i < ldap_count_values_len(vals); i++) {
                user_common_name += vals[i]->bv_val;
            }

            printf("LDAP search parameter_user found %s | %s\n", &parameter_user, &user_common_name);
            fflush(stdout);

            // free memory used for the values
            ldap_value_free_len(vals);
        }

        // free memory used for the attribute and the value structure
        ldap_memfree(attribute);
        if (ber != NULL) ber_free(ber, 0);
    }

    // free memory used for result
    ldap_msgfree(result);

    if (userDN != "") {
        // try to login
        BerValue *servercredp;
        BerValue cred;
        cred.bv_val = const_cast<char *> (parameter_password.c_str());
        cred.bv_len = parameter_password.length();

        int rc = ldap_sasl_bind_s(ld, userDN.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, &servercredp);

        if (rc != LDAP_SUCCESS) {
            printf("LDAP bind error %s", ldap_err2string(rc));
            fflush(stdout);
            return false;
        }

        printf("LDAP user bind successful");
        fflush(stdout);

        loginSuccess = true;
    } else {
        printf("LDAP error - no DN(distinguished name) for %s found", &parameter_user);
        fflush(stdout);
    }

    // unbind ldap
    ldap_unbind_ext_s(ld, NULL, NULL);

    return loginSuccess;
}

#endif //SERVER_LDAP_H
