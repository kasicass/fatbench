// Taken from https://gist.github.com/paulsmith/775764#file-echo-go

package main

import (
	"os"
	"log"
	"fmt"
	"net"
	"strconv"
	"flag"
	"runtime/pprof"
)

const PORT = 25000
var cpuprofile = flag.String("cpuprofile", "", "write cpu profile to file")

func main() {
	flag.Parse()
	if *cpuprofile != "" {
		f, err := os.Create(*cpuprofile)
		if err != nil {
			log.Fatal(err)
		}
		pprof.StartCPUProfile(f)
		defer pprof.StopCPUProfile()
		defer f.Close()
	}

	server, err := net.Listen("tcp", ":"+strconv.Itoa(PORT))
	if server == nil {
		panic("couldn't start listening: " + err.Error())
	}
	i := 0
	for {
		client, err := server.Accept()
		if client == nil {
			fmt.Printf("couldn't accept: " + err.Error())
			continue
		}
		i++
		fmt.Printf("%d: %v <-> %v\n", i, client.LocalAddr(), client.RemoteAddr())
		go handleConn(client)
	}
}

func handleConn(client net.Conn) {
	defer client.Close()
	buf := make([]byte, 102400)
	for {
		reqLen, err := client.Read(buf)
		if err != nil {
			break
		}
		if reqLen > 0 {
			client.Write(buf[:reqLen])
		}
	}
}
