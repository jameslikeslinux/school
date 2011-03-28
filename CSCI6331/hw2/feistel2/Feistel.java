import java.io.*;
import java.util.*;
import java.math.*;

public class Feistel {
    private static BigInteger messageSpaceSize;

    // pad bitstrings with zeros at the front
    private static String pad(String input, int length) {
        while (input.length() < length)
            input = "0" + input;

        return input;
    }

    private static BigInteger cyclicBitShiftLeft(BigInteger number, int count) {
        String string = number.toString(2);
        for (int i = 0; i < count; i++)
            string = string.substring(1) + string.charAt(0);
        return new BigInteger(string, 2);
    }

    /*
     * f(X_r, K) = X_r \times K^2 \mod (2^n)
     */ 
    private static BigInteger function(BigInteger x, BigInteger key) {
        return x.multiply(key.modPow(new BigInteger("2"), messageSpaceSize)).mod(messageSpaceSize);
    }

    private static BigInteger[] round(BigInteger messageL, BigInteger messageR, BigInteger key) {
        BigInteger[] output = new BigInteger[2];

        output[0] = messageR;
        output[1] = function(messageR, key).xor(messageL);

        return output;
    }

    public static void main(String[] args) throws Exception {
        Scanner in = new Scanner(new File("input_feistel.txt"));
        PrintWriter out = new PrintWriter(new FileOutputStream("output_feistel.txt"));

        // Read size of half-block
        int n = in.nextInt();
        messageSpaceSize = new BigInteger("2").pow(n);

        // Read key
        BigInteger inputKey = new BigInteger(in.next(), 2);
        
        // Read message
        String message = in.next();
        BigInteger messageL = new BigInteger(message.substring(0, n), 2);
        BigInteger messageR = new BigInteger(message.substring(n), 2);

        // Determine crypt type
        boolean decrypt = in.nextInt() == 1;
        
        BigInteger keys[] = new BigInteger[2];
        keys[0] = inputKey;
        keys[1] = cyclicBitShiftLeft(keys[0], 2);

        if (decrypt) {
            BigInteger tmp = messageR;
            messageR = messageL;
            messageL = tmp;

            tmp = keys[0];
            keys[0] = keys[1];
            keys[1] = tmp;
        }

        // Output size of half-block
        out.println("" + n);

        // Output key
        out.println(inputKey.toString(2));

        // Perform two rounds of the cipher
        BigInteger[] round = round(messageL, messageR, keys[0]);
        round = round(round[0], round[1], keys[1]);

        if (decrypt) {
            BigInteger tmp = round[0];
            round[0] = round[1];
            round[1] = tmp;
        }

        // Output message
        out.print(pad(round[0].toString(2), n));
        out.println(pad(round[1].toString(2), n));

        // Output opposite crypt type
        out.println(decrypt ? "0" : "1");

        out.close();
    }
}
