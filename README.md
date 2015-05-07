1. Name: Jennifer Wu

2. Email: jenniferwu0621@berkeley.edu

3. Platform: Linux (Ubuntu 14.04.2)

4. Compilation: Rule “make”.

5. Run-time instructions (run the following in order):

	1) Start Router
	`./router <port>`

	2) Start First Receiver
	`./receiver <router_port> <sender1_address> <sender1_port>`
			
	3) Start Second Receiver
	`./receiver <router_port> <sender2_address> <sender2_port> [-d]`
			
	4) Start First Sender
	`./sender <router_address> <router_port> <sender_port> <R> 1`
			
	5) Start Second Sender
	`./sender <router_address> <router_port> <sender_port> <R> 2`
	
