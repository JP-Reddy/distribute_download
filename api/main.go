package main

import (
    "encoding/json"
    "log"
    "net/http"
    "github.com/gorilla/mux"
    "fmt"
    "gopkg.in/cheggaaa/pb.v1"
    "time"
    "sync"
    "strconv"
)

type Progress struct {
    Index               int   `json:"index,omitempty"`
    // IP                  string   `json:"ip,omitempty"`
    Percentage          float32   `json:"percentage,omitempty"`
    Percent             int   `json:"percent,omitempty"`
    // TimeCompleted       string   `json:"time_completed,omitempty"`
}

var progress [3]Progress 

func ProgressBarSingle(){
    count := 100
    bar := pb.StartNew(count)
    second_bar := pb.StartNew(count)
    for i := 0; i < count; i++ {
        bar.Increment()
        second_bar.Increment()
        time.Sleep(time.Millisecond*100)
    }
    bar.FinishPrint("The End!")
    second_bar.FinishPrint("The End!")
}

func SendProgress(w http.ResponseWriter, r *http.Request) {
    params := mux.Vars(r)
    var ind int
    ind,_=strconv.Atoi(params["id"])
    // ind=int(params["id"])
    fmt.Println(params["id"])
    err := json.NewDecoder(r.Body).Decode(&progress[ind])
    if err != nil {
        panic(err)
    }
    // progress.Percentage = params["percentage"]
    progress[ind].Index=ind
    progress[ind].Percent=int(progress[ind].Percentage)
    fmt.Println( progress[ind].Index)
    // fmt.Println(progress.IP)
    fmt.Println( progress[ind].Percentage)
    
}

func StartProgressBar(w http.ResponseWriter, r *http.Request) {
    ProgressBarsMultiple()
}

func ProgressBarsMultiple(){
    
    first := pb.New(100).Prefix("First ")
    second := pb.New(100).Prefix("Second ")
    third := pb.New(100).Prefix("Third ")
    // start pool
    pool, err := pb.StartPool(first, second, third)
    if err != nil {
        panic(err)
    }
    // update bars
    wg := new(sync.WaitGroup)
    var i int
    i=0
    var cur_prog = [3]int{0,0,0} 
        
    for _, bar := range []*pb.ProgressBar{first, second, third} {
        wg.Add(1)
        go func(cb *pb.ProgressBar,i int) {
            for true {
                // fmt.Println(progress[i].Percent)
                if progress[i].Percent==100{
                    break
                }
                if progress[i].Percent > cur_prog[i] {
                    for x:=0; x<(progress[i].Percent-cur_prog[i]);x++{
                        cb.Increment()
                    }
                    cur_prog[i]=progress[i].Percent
                }
            }
            
            cb.Finish()
            wg.Done()
            }(bar,i)
        i+=1
    }
    
    wg.Wait()
    // close pool
    pool.Stop()
}

// our main function
func main() {
    router := mux.NewRouter()
    router.HandleFunc("/progress/{id}", SendProgress).Methods("POST")
    //router.HandleFunc("/people/{id}", GetPerson).Methods("GET")
    //router.HandleFunc("/people/{id}", DeletePerson).Methods("DELETE")
    router.HandleFunc("/start", StartProgressBar).Methods("GET")
    log.Fatal(http.ListenAndServe(":9517", router))
    // ProgressBarsMultiple()
}

