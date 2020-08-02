obj = main.cpp cthread_pool.h

main: $(obj)
	g++ $(obj) -o main -std=c++11 -lpthread

.PHONY: clean
clean:
	rm main
