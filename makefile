P1 : a
	./vm test.txt > out.txt
	./vm test.txt
a :
	gcc -o vm vm.c
big:
	./vm addresses.txt > outbig.txt
	./vm addresses.txt
clean:
	rm vm
	rm out.txt
	rm outbig.txt
