make: sender.c receiver.c packet.c timer.c
	gcc -o sender sender.c packet.c timer.c -I. -pthread
	gcc -o receiver receiver.c packet.c timer.c -I. -pthread

sender: sender.c packet.c timer.c
	gcc -o sender sender.c packet.c timer.c -I. -pthread

receiver: receiver.c packet.c timer.c
	gcc -o receiver receiver.c packet.c timer.c -I. -pthread
     
