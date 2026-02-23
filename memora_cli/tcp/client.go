package tcp

import (
	"fmt"
	"net"
)

const max_payload_size = 1024

type tcpClient struct {
	addr string
	conn net.Conn
}

func NewTcpClient(addr string) (*tcpClient, error) {
	if addr == "" {
		return nil, fmt.Errorf("address is required")
	}

	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return nil, fmt.Errorf("Could not connect to memora server. Please check if server is running.")
	}

	return &tcpClient{
		addr: addr,
		conn: conn,
	}, nil
}

func (t *tcpClient) Send(payload string) error {
	if payload == "" {
		return fmt.Errorf("Payload is empty")
	}

	payloadBytes := []byte(payload)

	_, err := t.conn.Write(payloadBytes)
	if err != nil {
		return fmt.Errorf("write operation is failed")
	}
	return nil
}

func (t *tcpClient) Read() (string, error) {
	bytes := make([]byte, max_payload_size)

	n, err := t.conn.Read(bytes)
	if err != nil {
		return "", fmt.Errorf("read operation is failed")
	}

	return string(bytes[:n]), nil
}

func (t *tcpClient) Close() {
	t.conn.Close()
}
