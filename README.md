1. Name: Jennifer Wu

2. Email: jenniferwu0621@berkeley.edu

3. Platform: Linux (Ubuntu 14.04.2)

4. Compilation: Rule “make”.

5. Run-time instructions (run the following in order):

	1) Start Router
		`./router <port>`

	2) Start First Receiver
	`./receiver <router_addr> <port>`
			
	3) Start Second Receiver
	`./receiver <router_addr> <port>`
			
	4) Start First Sender
	`./sender <router_addr> <port> <R> <sender_id>`
			
	5) Start Second Sender
	`./sender <router_addr> <port> <R> <sender_id>`
	
