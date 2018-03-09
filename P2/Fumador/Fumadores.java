import monitor.*;
import java.util.Random;

class Estanco extends AbstractMonitor {
	private int ingredienteMesa;
	private Condition ingrediente1 = makeCondition();
	private Condition ingrediente2 = makeCondition();
	private Condition ingrediente3 = makeCondition();
	private Condition mesaOcupada = makeCondition();

	//El fumador pide al estanquero su ingrediente
	public void obtenerIngrediente(int miIngrediente){			
		enter();
			if(miIngrediente==0 && ingredienteMesa==-1)
				ingrediente1.await();
			else if(miIngrediente==1 && ingredienteMesa==-1)
				ingrediente2.await();
			else if(miIngrediente==2 && ingredienteMesa==-1)
				ingrediente3.await();

			ingredienteMesa=-1;
		leave();	
	}

	//El estanquero pone un ingrediente en la mesa
	public void ponerIngrediente(int ingrediente){
		enter();
			if(ingredienteMesa!=-1)
				mesaOcupada.await();
				
			if(ingrediente==0)
				ingrediente1.signal();
			if(ingrediente==1)
				ingrediente2.signal();
			if(ingrediente==2)
				ingrediente3.signal();

		leave();
	}

	//El estanquero se espera a que un fumador recoja el ingrediente
	public void esperarRecogidaIngrediente() {
		enter();
			while(ingredienteMesa!=-1) {}
			mesaOcupada.signal();
		leave();
	}

}

class Fumador implements Runnable {
	private int nFumador;
	private int miIngrediente;
	public Thread thr;
	Estanco estanco;

	public Fumador(String name,Estanco estanco,int nFumador) {
		this.nFumador=nFumador;
		this.miIngrediente=nFumador%3;
		this.estanco=estanco;
		thr = new Thread(this,name);
	}

	public void run() {
		while(true) {
			estanco.obtenerIngrediente(miIngrediente);
			System.out.print(thr.getName()+": esta fumando");
			aux.dormir_max(2000);
			System.out.print(thr.getName()+": ha dejado de fumar");
		}
	}
}

class Estanquero implements Runnable {
	Estanco estanco;
	public Thread thr;
	

	public Estanquero(Estanco estanco) {
		this.estanco=estanco;
		thr = new Thread(this,"Estanco");
	}

	public void run() {
		int ingrediente;
		Random rnd = new Random();

		while(true) {
			ingrediente= rnd.nextInt()%3;
			estanco.ponerIngrediente(ingrediente);
			System.out.print("El estanquro pone en la mesa el ingrediente " + ingrediente);
			estanco.esperarRecogidaIngrediente();
		}
	}
}

public class Fumadores{

	public static void main(String[] args) {
		if(args.length==1) {
				int nFumadores=Integer.parseInt(args[2]);

				Estanco estanco;

				Estanquero estanquero= new Estanquero(estanco);
				Fumador[] fumadores = new Fumador[nFumadores];
	
				for(int i=0;i<nFumadores;i++)
					fumadores[i]= new Fumador("Fumador "+i,estanco,i);

				estanquero.thr.start();
				for(int i=0;i<nFumadores;i++)
					fumadores[i].thr.start();

		}else
			System.out.print("Uso:\n\t1. Numero de fumadores");
	}
}

