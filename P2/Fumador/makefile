all: Fumadores.class

Fumadores.class: monitor
	javac *.java

monitor:
	javac monitor/*.java

install:
	sudo apt-get install ecj
mrproper:
	rm -f monitor/*.class
	rm -f *.class
