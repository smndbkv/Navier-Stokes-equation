FLAGS = -I/usr/include/eigen3 -isystem /usr/lib/x86_64-linux-gnu/openmpi/include -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format 
FLAGS_UNUSED = -I/usr/include/eigen3 -isystem /usr/lib/x86_64-linux-gnu/openmpi/include -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
FLAGS_DEBUG = -fsanitize=address -I/usr/include/eigen3 -isystem /usr/lib/x86_64-linux-gnu/openmpi/include -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format 

ZIP_NAME = Dubkov_SA_CourseWork
all: a.out

a.out: *.cpp *.h linearsystem/GaussianMethod/*.cpp linearsystem/IterativeMethod/*.cpp
	g++ $(FLAGS) $^ -o a.out

unused: *.cpp *.h linearsystem/GaussianMethod/*.cpp linearsystem/IterativeMethod/*.cpp
	g++ $(FLAGS_UNUSED) $^ -o a.out

debag: *.cpp *.h linearsystem/GaussianMethod/*.cpp linearsystem/IterativeMethod/*.cpp
	g++ $(FLAGS_DEBUG) $^ -o a.out

zip:
	zip $(ZIP_NAME).zip *.h *.cpp Makefile 

clean:
	rm a.out 

test: a.out
	@echo "Running tests..."
	@for file in $(TEST_FILES); do \
		echo "Testing with $$file:"; \
		./a.out $(N) $(M) $(P) $(R) 0 $$file; \
		echo "---"; \
	done

.PHONY: test clean