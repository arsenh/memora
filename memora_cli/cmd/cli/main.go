package main

import (
	"bufio"
	"fmt"
	"log"
	"os"

	"github.com/arsenh/memora_cli/tcp"
)

func main() {
	// parser := resp.NewRESPParser()

	// commands := []string{
	// 	"PING",
	// 	"SET key value",
	// 	"GET key",
	// 	"DEL key",
	// 	"EXISTS key",
	// 	"MSET a 1 b 2 c 3",
	// 	"LPUSH mylist b c d",
	// }

	// for _, command := range commands {
	// 	payload := parser.Parse2(command)
	// 	fmt.Printf("Client command: %s, Payload: %q\n", command, payload)
	// }

	fmt.Println("Memora CLI.")
	serverAddress := "localhost:8080"

	client, err := tcp.NewTcpClient(serverAddress) // TODO: need to get address from command line

	if err != nil {
		log.Fatal(err)
	}

	defer client.Close()
	fmt.Printf("Connected to Memora server at %s\n", serverAddress)

	for {
		reader := bufio.NewReader(os.Stdin)
		fmt.Print(">> ")
		input, _ := reader.ReadString('\n')

		if input == "quit" {
			log.Println("Exist.")
			os.Exit(0)
		}

		err := client.Send(input)
		if err != nil {
			log.Fatal(err)
		}

		response, err := client.Read()
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println(response)
	}
}
