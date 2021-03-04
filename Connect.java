import java.net.*;
import java.io.*;

public class Connect{
	public static void main(String[] args) throws Exception{
		if(args.length!=2) {
			System.exit(1);
		}
		Socket s = new Socket(args[0],Integer.parseInt(args[1]));
		OutputStream os = s.getOutputStream();
		while(true) {
			os.write(System.in.read());
		}
	}
}
