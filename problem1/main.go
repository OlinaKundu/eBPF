package main
import (
	"fmt"
	"log"
	"os"
	"strconv"
	"github.com/cilium/ebpf"
)

func main(){
	if len(os.Args) != 2 {
		log.Fatalf("Usage: %s <port>", os.Args[0])
	}
	port, err := strconv.ParseUint(os.Args[1], 10, 16)
	if err != nil || port ==0 {
		log.Fatal("Port must be a number between 1 and 65535")
	}
	m, err:= ebpf.LoadPinnedMap(
		"/sys/fs/bpf/config_map",
		nil,
	)
	if err != nil{
		log.Fatalf("Open pinned map: %v", err)
	}
	defer m.Close()
	key := uint32(0)
	value := uint16(port)
	if err := m.Update(&key, &value, ebpf.UpdateAny); err != nil{
		log.Fatalf("Update port: %v", err)
	}
	fmt.Printf("Configured XDP filter to drop TCP destination port %d\n", port)
}
