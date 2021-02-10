import java.net.*;
public class Connect{
	public static void main(String[] args){
		if(args.length!=2)System.exit(1);
		try{
			new Socket(args[0],Integer.parseInt(args[1]));
		}catch(Exception e){e.printStackTrace();}
	}
}
