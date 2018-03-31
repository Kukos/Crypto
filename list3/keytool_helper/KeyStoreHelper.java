import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.security.KeyStore;
import java.security.KeyStore.Entry;
import java.security.KeyStore.LoadStoreParameter;
import java.security.KeyStore.ProtectionParameter;
import java.security.KeyStore.SecretKeyEntry;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.UnrecoverableEntryException;
import java.security.cert.CertificateException;

import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
import java.util.Random;

public class KeyStoreHelper
{

	static public String randIv()
	{
		Random rand = new Random();
		int n = 32;
		String iv = "";

		for (int i = 0; i < n; ++i)
			iv += (char)rand.nextInt(26) + 'a';

		return iv;
	}

	static public void storeKey(KeyStore ks,KeyStore.ProtectionParameter entryPassword, String alias)
	{
		String iv = randIv();
		try {
			if(!ks.containsAlias(alias))
			{
				SecretKey secretKey= new SecretKeySpec(iv.getBytes(), "aes");
				KeyStore.SecretKeyEntry secretKeyEntry = new KeyStore.SecretKeyEntry(secretKey);
				try {
					ks.setEntry(alias, secretKeyEntry, entryPassword);
				} catch (KeyStoreException e) {
					System.out.println("fail reason " + e.getMessage());
				}
			}
		} catch (KeyStoreException e) {
			System.out.println("fail reason " + e.getMessage());
		}
	}

	static public void getKey(KeyStore ks, KeyStore.ProtectionParameter entryPassword, String alias)
	{
		try {
			KeyStore.SecretKeyEntry keyEntry = (SecretKeyEntry) ks.getEntry(alias, entryPassword);
			String key = new String(keyEntry.getSecretKey().getEncoded());
			System.out.println(key);
		} catch (NoSuchAlgorithmException e) {
			System.out.println("fail reason "+e.getMessage());
		} catch (UnrecoverableEntryException e) {
			System.out.println("fail reason "+e.getMessage());
		} catch (KeyStoreException e) {
			System.out.println("fail reason "+e.getMessage());
		}
	}

	public static void help()
	{
		System.out.println("\t\tKEYSTORE HELPER\n\n");
		System.out.println("PARAMS\n");
		System.out.println("--get / -g\tGet secret key");
		System.out.println("--store / -s\tSave secret key");
		System.out.println("--password / -p\tSecret key password");
		System.out.println("--file / -f\tPath to file");
		System.out.println("--alias /-a\tKey alias");
	}

	public static void main(String[] args)
	{
		final int MODE_GET = 0;
		final int MODE_STORE = 1;
		int mode = -1;
		String password = "";
		String alias = "";
		String file = "";

		try {
			if(args.length < 4)
			{
				help();
				System.exit(0);
			}

			for (int i = 0; i < args.length - 1; ++i)
			{
				String s = args[i];
				String ss = args[i + 1];
				if (s.equals("--get") || s.equals("-g"))
					mode = MODE_GET;

				if (s.equals("--store") || s.equals("-s"))
					mode = MODE_STORE;

				if (s.equals("--password") || s.equals("-p"))
					password = ss;

				if (s.equals("--file") || s.equals("-f"))
					file = ss;

				if (s.equals("--alias") || s.equals("-a"))
					alias = ss;
			}

			KeyStore.ProtectionParameter entryPassword = new KeyStore.PasswordProtection(password.toCharArray());
			KeyStore keys = KeyStore.getInstance("PKCS12");
			File f = new File(file);

				try {
					if(f.exists() && !f.isDirectory())
						keys.load(new FileInputStream(file), password.toCharArray());
					else
						keys.load(null, null);
				} catch (NoSuchAlgorithmException e1) {
					System.out.println("fail reason " + e1.getMessage());
				} catch (CertificateException e1) {
					System.out.println("fail reason " + e1.getMessage());
				} catch (FileNotFoundException e1) {
					System.out.println("fail reason " + e1.getMessage());
				} catch (IOException e1) {
					System.out.println("fail reason " + e1.getMessage());
				}

			switch (mode)
			{
				case MODE_GET:
					getKey(keys, entryPassword, alias);
					break;
				case MODE_STORE:
					storeKey(keys, entryPassword, alias);

			}


			FileOutputStream out;
			try {
				out = new FileOutputStream(file);

				try {
					keys.store(out, password.toCharArray());
				} catch (NoSuchAlgorithmException e) {
					System.out.println("fail reason " + e.getMessage());
				} catch (CertificateException e) {
					System.out.println("fail reason " + e.getMessage());
				} catch (IOException e) {
					System.out.println("fail reason " + e.getMessage());
				}

			    out.close();

			} catch (IOException e) {
				System.out.println("fail reason " + e.getMessage());
			}
		} catch (KeyStoreException e) {
			System.out.println("fail reason " + e.getMessage());
		}
	}
}