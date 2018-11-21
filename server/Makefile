CC = g++
CFLAGS = -std=c++11 -c -g -Wall
LFLAGS = -std=c++11 -g -Wall -pthread
OBJECTS = commons/helper.o commons/ldap.o commons/messages.o commons/string_code.o commons/thread_args.o handler/mail_handler.o myserver.o
LDAP = -lldap -llber

all: server

commons/helper.o: commons/helper.h
	$(CC) $(CFLAGS) -o commons/helper.o commons/helper.h
commons/ldap.o: commons/ldap.h
	${CC} ${CFLAGS} -o commons/ldap.o commons/ldap.h
commons/messages.o: commons/messages.h
	${CC} ${CFLAGS} -o commons/messages.o commons/messages.h
commons/string_code.o: commons/string_code.h
	${CC} ${CFLAGS} -o commons/string_code.o commons/string_code.h
commons/thread_args.o: commons/thread_args.h
	${CC} ${CFLAGS} -o commons/thread_args.o commons/thread_args.h

handler/mail_handler.o: handler/mail_handler.h
	${CC} ${CFLAGS} handler/mail_handler.h -o handler/mail_handler.o

myserver.o: myserver.cpp
	${CC} ${CFLAGS} myserver.cpp -omyserver.o

server: $(OBJECTS)
	${CC} $(OBJECTS) -o server $(LDAP)


clean:
	rm -f $(OBJECTS) server

# g++ options:
# -g        debugging information
# -Wall     show all warnings
# -c        compile only, no linking
# -o        specify output
