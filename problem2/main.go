package main
import (
"fmt"
"log"
"os"
"strconv"

"github.com/cilium/ebpf"
)

func main() {
	port := uint64(4040)
	if len(os.Args) > 1{
		var err error
		port, err = strconv.ParseUint(os.Args[1],10,16)
		if err != nil || port == 0{
			log.Fatal("Port must be between  and 65535")
		}
	}
	m,err := ebpf.LoadPinnedMap("/sys/fs/bpf/config_map", nil)
	if err != nil {
		log.Fatal("Open pinned map: %v", err)
	}
	defer m.Close()
	key := uint32(0)
	value:=uint16(port)
	if err:= m.Update(&key, &value, ebpf.UpdateAny); err != nil {
		log.Fatal("Update port : %v", err)
	}
	fmt.Printf("Allowed TCP destination port set to %d\n", port)
}
