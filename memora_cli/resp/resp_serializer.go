package resp

import (
	"fmt"
	"log"
	"math"
	"strconv"
	"strings"
)

type RESPParser struct {
	payload string
}

const END_OF_LINE string = "\r\n"

const (
	resp_type_simple_string = "+"
	resp_type_bulk_strings  = "$"
	resp_type_bulk_arrays   = "*"
	respo_type_big_numbers  = "("
)

const (
	chunk_string = iota
	chunk_int
	chunk_float
)

func NewRESPParser() *RESPParser {
	return &RESPParser{}
}

func detectType(s string) int {
	// Try integer
	if _, err := strconv.Atoi(s); err == nil {
		return chunk_int
	}

	// Try float
	if _, err := strconv.ParseFloat(s, 64); err == nil {
		return chunk_float
	}

	// Otherwise, treat as text
	return chunk_string
}

func parse_chunk_string(chunk string) string {
	return fmt.Sprintf("%s%d%s%v%s", resp_type_bulk_strings, len(chunk), END_OF_LINE, chunk, END_OF_LINE)
}

func parse_chunk_big_numbers(chunk string) string {
	value, err := strconv.Atoi(chunk)
	if err != nil {
		log.Fatal("Incorrect data format, must integer value")
	}

	sign := func(v int) string {
		if v >= 0 {
			return "+"
		} else {
			return "-"
		}
	}(value)

	return fmt.Sprintf("%s%s%d,%s", respo_type_big_numbers, sign, int(math.Abs(float64(value))), END_OF_LINE)

}

func (r *RESPParser) Parse(input string) (string, error) {
	builder := strings.Builder{}
	chunks := strings.Split(input, " ")

	// parse resp_type_bulk_arrays
	fmt.Fprintf(&builder, "%s%d%s", resp_type_bulk_arrays, len(chunks), END_OF_LINE)

	for _, chunk := range chunks {
		switch detectType(chunk) {
		case chunk_string:
			fmt.Fprintf(&builder, "%s", parse_chunk_string(chunk))
		case chunk_int:
			fmt.Fprintf(&builder, "%s", parse_chunk_big_numbers(chunk))
		default:
			panic("Not implemented for this type")
		}
	}

	return builder.String(), nil
}

func (r *RESPParser) Parse2(input string) string {
	builder := strings.Builder{}
	chunks := strings.Split(input, " ")

	fmt.Fprintf(&builder, "%s%d%s", resp_type_bulk_arrays, len(chunks), END_OF_LINE)
	for _, chunk := range chunks {
		fmt.Fprintf(&builder, "%s", parse_chunk_string(chunk))
	}
	return builder.String()
}
