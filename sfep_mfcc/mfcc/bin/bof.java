
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;


/**
 * This script will generate a bag of words fromt the tokens(txyc) file 
 * It will count the times each "word" has appeared in the txyc file
 * Since when you generate the txyc file, you can store upto topK cluster
 * centers (word) for each feature point. So, when we count the times for
 * each word, we have two different ways to compute: soft and hard, where
 * soft means we will take the weighted average on all the topK word, and
 * hard means we only count the first word that each feature point belongs
 * to.
 * @author srawat
 */

public class bof {
    
    private static String DELIM=" ";
    public static String FILE_TYPE = ".bof";
    
    private String TokensFile;
    private String BOFFile;
    private int TopK; // topk words to consider
    private int CentersCount;//no of cluster centers
    
    public bof(String file,int noOfClusters,int topK,String outPath){
        this.CentersCount = noOfClusters;
        this.TopK = topK;
        this.TokensFile = file;
	String temp = (new File(file)).getName();
        this.BOFFile = outPath+"/"+temp.substring(0,temp.lastIndexOf('.'))+FILE_TYPE;
        //this.BOFFile = outPath+"/"+(new File(file)).getName().split("\\.")[0]+FILE_TYPE;
    }
    
    private void generateBOF() throws FileNotFoundException, IOException{
        BufferedWriter writer = 
                new BufferedWriter(new OutputStreamWriter(new FileOutputStream(this.BOFFile)));
        BufferedReader reader = 
                new BufferedReader(new InputStreamReader(new FileInputStream(this.TokensFile)));
        
        float[] histogram = new float[this.CentersCount];
        for(int i=0;i<this.CentersCount;i++){
            histogram[i] = 0;
        }
        
        int instances = 0;
        String str="";
        while((str=reader.readLine())!=null){
            instances++;
            String[] points = str.trim().split(DELIM);
            for(int i=0;i<this.TopK;i++){
                String feature = points[i];
                int featureIndex = Integer.parseInt(feature);
                histogram[featureIndex] += (1.0/(float)(i+1));
            }
        }
        //normalize the histogram weights
        float normalizer = 0;
        for(int i=0;i<this.TopK;i++){
            normalizer += (1.0/(i+1));
        }
        normalizer *= instances;
        
        //start printing the stuff out
        String output = "";
        for(int i=0;i<this.CentersCount;i++){
            if(histogram[i]==0){
                continue;
            }
            histogram[i] = histogram[i]/normalizer;
            output += " "+(i+1)+":"+histogram[i];
        }
        
        writer.write(output.trim());
        writer.newLine();writer.flush();
        writer.close();
        System.out.println("BOF generation complete. "+this.BOFFile);
        reader.close();
    }
    
    public static void main(String[] args) throws FileNotFoundException, IOException{
        if(args.length!=4){
            usage();
            System.exit(1);
        }
        
        String txycFile = args[0];
        int noOfClusters = Integer.parseInt(args[1]);
        int TopK = Integer.parseInt(args[2]);
        String outPath = args[3];
        bof obj = new bof(txycFile,noOfClusters,TopK,outPath);
        obj.generateBOF();
    }
    
    public static void usage(){
        String message = "Usage: bof <PathTotxycFile> <No of Clusters> <TopK> <Output Directory>\n";
        System.out.println(message);
    }
    
}
