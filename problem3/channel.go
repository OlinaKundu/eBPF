package main
import "fmt"
func main(){
	ch:= make(chan string)
	go func(){
		ch <- "Hello from worker"
	}()
	message := <- ch
	fmt.Println(message)
}
