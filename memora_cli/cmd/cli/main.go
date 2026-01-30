package main

import (
	"bufio"
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

const K_MAX_MSG_LEN = 4096
const HEADER_LEN = 4

func query(conn net.Conn, text string) error {
	l := len(text)
	if l > K_MAX_MSG_LEN {
		return errors.New("payload too large")
	}
	textLen := uint32(l)
	writeData := make([]byte, HEADER_LEN+textLen)

	binary.LittleEndian.PutUint32(writeData[0:HEADER_LEN], textLen)
	copy(writeData[HEADER_LEN:], text)

	n, err := conn.Write(writeData)
	if err != nil {
		log.Println("error to write data to server")
		return err
	}
	log.Printf("sended bytes: %d\n", n)

	header := make([]byte, 4)
	n, err = io.ReadFull(conn, header)
	if err != nil {
		log.Println("error readfull from server")
		return err
	}
	log.Printf("recevied header bytes: %d\n", n)

	len := binary.LittleEndian.Uint32(header)

	if len > K_MAX_MSG_LEN {
		log.Println("error payload is too large")
		return errors.New("payload error")
	}

	payload := make([]byte, len)

	io.ReadFull(conn, payload)

	fmt.Println("Payload: ", string(payload))
	return nil
}

func main() {
	fmt.Println("Memora CLI.")
	serverAddress := "localhost:8080"

	conn, err := net.Dial("tcp", serverAddress)

	if err != nil {
		log.Fatalln("Could not connect to memora server. Please check if server is running.")
	}

	defer conn.Close()

	fmt.Printf("Connected to Memora server at %s\n", serverAddress)

	for {
		reader := bufio.NewReader(os.Stdin)
		fmt.Print(">> ")
		intput, _ := reader.ReadString('\n')

		if intput == "quit" {
			log.Println("Exist.")
			os.Exit(0)
		}

		err := query(conn, intput)

		if err != nil {
			break
		}
	}
}
