package main

import (
    "encoding/json"
    "log"
    "net/http"
    "github.com/gorilla/mux"
    "fmt"
    "gopkg.in/cheggaaa/pb.v1"
    "time"
    "math/rand"
    "sync"
)

type Progress struct {
    Index               string   `json:"index,omitempty"`
    // IP                  string   `json:"ip,omitempty"`
    Percentage          float32   `json:"percentage,omitempty"`
    // TimeCompleted       string   `json:"time_completed,omitempty"`
}

var progress Progress 

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

    fmt.Println(params)
    err := json.NewDecoder(r.Body).Decode(&progress)
    if err != nil {
        panic(err)
    }
    // progress.Percentage = params["percentage"]
    progress.Index=params["id"]
    fmt.Println("Index: "progress.Index)
    // fmt.Println(progress.IP)
    fmt.Println("Percentage: "+progress.Percentage)

    ProgressBarSingle()
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
    for _, bar := range []*pb.ProgressBar{first, second, third} {
        wg.Add(1)
        go func(cb *pb.ProgressBar) {
            for n := 0; n < 100; n++ {
                cb.Increment()
                time.Sleep(time.Millisecond * time.Duration(rand.Intn(100)))
            }
            cb.Finish()
            wg.Done()
        }(bar)
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
    log.Fatal(http.ListenAndServe(":9517", router))
}

