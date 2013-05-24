default:
	rm -f Lush.hs
	rm -f Scanner.hs
	alex Scanner.x
	happy Lush.y
	ghc -o lush Scanner.hs Lush.hs

clean:
	rm -f *.hs
	rm -f *.hi
	rm -f *.o
	rm -f lush
