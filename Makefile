all: serial threaded openmp mpi threaded_both_2_loop 
serial: serial.cpp
	g++ -std=c++11 -o serial serial.cpp -I libarff libarff/arff_attr.cpp libarff/arff_data.cpp libarff/arff_instance.cpp libarff/arff_lexer.cpp libarff/arff_parser.cpp libarff/arff_scanner.cpp libarff/arff_token.cpp libarff/arff_utils.cpp libarff/arff_value.cpp
threaded: threaded.cpp
	g++ -pthread -std=c++11 -o threaded threaded.cpp -I libarff libarff/arff_attr.cpp libarff/arff_data.cpp libarff/arff_instance.cpp libarff/arff_lexer.cpp libarff/arff_parser.cpp libarff/arff_scanner.cpp libarff/arff_token.cpp libarff/arff_utils.cpp libarff/arff_value.cpp
threaded_both_2_loop: threaded_both_2_loop.cpp
	g++ -pthread -std=c++11 -o threaded_both_2_loop threaded_both_2_loop.cpp -I libarff libarff/arff_attr.cpp libarff/arff_data.cpp libarff/arff_instance.cpp libarff/arff_lexer.cpp libarff/arff_parser.cpp libarff/arff_scanner.cpp libarff/arff_token.cpp libarff/arff_utils.cpp libarff/arff_value.cpp
openmp: openmp.cpp
	g++ -fopenmp -std=c++11 -o openmp openmp.cpp -I libarff libarff/arff_attr.cpp libarff/arff_data.cpp libarff/arff_instance.cpp libarff/arff_lexer.cpp libarff/arff_parser.cpp libarff/arff_scanner.cpp libarff/arff_token.cpp libarff/arff_utils.cpp libarff/arff_value.cpp
mpi: mpi.cpp
	mpicxx -std=gnu++11 -o mpi mpi.cpp -I libarff libarff/arff_attr.cpp libarff/arff_data.cpp libarff/arff_instance.cpp libarff/arff_lexer.cpp libarff/arff_parser.cpp libarff/arff_scanner.cpp libarff/arff_token.cpp libarff/arff_utils.cpp libarff/arff_value.cpp
clean:
	rm serial threaded openmp mpi threaded_both_2_loop
