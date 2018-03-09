import monitor.* ;


class Buffer extends AbstractMonitor {
	private final int N=20;		//Tamaño del buffer
	private int[] buffer = new int[N];		
	private int nItems=0;		//Numero de items que hay en el buffer
	private Condition empty = makeCondition();	//Indica si el buffer esta vacio
	private Condition full  = makeCondition();	//Indica si el buffer esta lleno
	
	
	void deposita(int item) {
		enter();

			if(nItems==N)
				full.await();

			buffer[nItems]=item;
			nItems++;

			if(nItems==1)
				empty.signal();
		
		leave();
	}


	int extrae() {
		enter();
			int item;
			
			if(nItems==0)
				empty.await();

			nItems--;
			item=buffer[nItems];

			if(nItems==N-1)
				full.signal();

		leave();
		return item;
	}
}

class Productor implements Runnable {
	private Buffer bufferPC;
	private int nVeces;
	private int nProductor;
	public Thread thr;

	public Productor(String nombre,Buffer bufferPC,int nProductor,int nVeces) {
		this.bufferPC=bufferPC;
		this.nProductor=nProductor;
		this.nVeces=nVeces;
		thr=new Thread(this,nombre);
	}

	public void run() {
		int i;

		for(i=nProductor*nVeces;i<nVeces*(nProductor+1);i++) {
			bufferPC.deposita(i);
			System.out.print("Productor "+nProductor+": produce "+i+"\n");
		}
	}
}

class Consumidor implements Runnable {
	private Buffer bufferPC;
	private int nVeces;
	private int nConsumidor;
	public Thread thr;

	public Consumidor(String nombre,Buffer bufferPC,int nConsumidor,int nVeces){
		this.bufferPC=bufferPC;
		this.nConsumidor=nConsumidor;
		this.nVeces=nVeces;
		thr=new Thread(this,nombre);
	}
				

	public void run() {
		int item;
		int i;
	
		for(i=0;i<nVeces;i++){
			item=bufferPC.extrae();
			System.out.print("\t\t\t\t\tConsumidor "+nConsumidor+": consume "+item+"\n");
		}
	}
}	

public class ProductorConsumidor {
	public static void main(String[] args) {
 
		if ( args.length == 4 ) { 
			int nConsumidores=Integer.parseInt(args[0]);
			int nProductores=Integer.parseInt(args[1]);
			int nConsumiciones=Integer.parseInt(args[2]);
			int nProducciones=Integer.parseInt(args[3]);

			if(nProductores*nProducciones==nConsumidores*nConsumiciones) {
			    
				Consumidor[] cons = new Consumidor[nConsumidores];
				Productor[]  prod = new Productor[nProductores];

				Buffer buffer = new Buffer();
				 
		   
				for( int i = 0; i < cons.length; i++) 
					cons[i] = new Consumidor("Consumidor "+(i+1),buffer,i,nConsumiciones);

				for( int i = 0; i < prod.length; i++) 
				    	prod[i] = new Productor("Productor"+(i+1),buffer,i,nProducciones);
				    
				for( int i = 0; i < cons.length ; i++) 
				      cons[i].thr.start();

				for( int i = 0; i < prod.length ; i++) 
				      prod[i].thr.start();

			}else	//nProductores*nProducciones!=nConsumidores*nConsumiciones
				System.err.println("El numero de elementos consumidos debe coincidir"+
						    " con el numero de elementos producidos");
		}else	//args.length != 4 
			System.err.println("Uso: Intruducir 4 argumentos\n\t1. Numero Consumidores\n\t2. Numero Productores"+
					         "\n\t3. Numero Consumiciones\n\t4. Numero Producciones");
	}
}

			
		
		
		
