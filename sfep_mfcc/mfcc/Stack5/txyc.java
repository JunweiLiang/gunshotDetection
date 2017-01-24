
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.io.File;
import java.util.zip.GZIPInputStream;

/**
 * This will generate the tokens based on TopK closest cluster centers
 * @author srawat
 */
public class txyc {
    
    public static String FILE_DELIM = "[ ]+"; // one or more dashes
    public static String FILE_TYPE = ".txyc";
    public static int TOPK_DEFAULT = 10;
    
    private ArrayList<double[]> centerList;
    private int TopK;
    private int ClusterCount;
    private int FeatureCount;
    private String DataFile;
    private String TokensFile;
    
    public txyc(String centroidFile,String dataFile, int topK,String outPath) throws FileNotFoundException, IOException{
        //Initialize the Centroids
        centerList = new ArrayList<double[]>();
        this.initCentroids(centroidFile);
        this.TopK = topK;
        this.DataFile = dataFile;
        this.TokensFile = outPath+"/"+(new File(dataFile)).getName().split("\\.")[0]+FILE_TYPE;
        System.out.println(this.TokensFile);
    }
    
    public static void main(String[] args) throws FileNotFoundException, IOException{
        if(args.length!=4){
            usage();
            System.exit(1);
        }
        
        String centroidFile = args[0];
        String dataFile = args[1];
        //take 10 as the default TopK value
        int TopK = (args[2]!=null) ? Integer.parseInt(args[2]):TOPK_DEFAULT;
        String outPath = args[3];
        txyc obj = new txyc(centroidFile,dataFile,TopK,outPath);
        obj.generateTokensStack5();
    }
    
    //this will read in the KMeansCenters file
    //TODO: validate that the KMeans Centers file is correct
    private void initCentroids(String file) throws FileNotFoundException, IOException{
        System.out.println("Reading the centroid file ..");
        BufferedReader reader = 
                new BufferedReader(new InputStreamReader(new FileInputStream(file)));
        String line = "";
        while((line=reader.readLine())!=null){
            String[] centers = line.trim().split(FILE_DELIM);
            int index = 0;
            double[] centersDouble = new double[centers.length]; 
            while(index<centers.length){
                centersDouble[index] = Double.parseDouble(centers[index]);
                index++;
            }
            this.centerList.add(centersDouble);
        }
        this.ClusterCount = centerList.size();
        this.FeatureCount = centerList.get(0).length;
        reader.close();
    }
    
    //this will generate the tokens and write it to the output .txyc file
    public void generateTokens() throws FileNotFoundException, IOException{
        BufferedWriter writer = null;
        if(new File(this.TokensFile).exists()){
        	return;
        } else {
        	writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(this.TokensFile))); 
        }
        
        BufferedReader reader = null;
        if(this.DataFile.endsWith(".gz")){
        	//this is a gzip file
        	reader = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(this.DataFile))));
        }else{
        	//this is just a normal text file
        	reader = new BufferedReader(new InputStreamReader(new FileInputStream(this.DataFile)));
        }
        String row="";
        while((row=reader.readLine())!=null){
            //convert to a double array
            double[] columns = this.string2Double(row.trim().split(FILE_DELIM));
            if(columns.length!=this.FeatureCount){
                System.err.println("Format error in input file");
                System.exit(1);
            }
            double[] distances = this.calcDistance(columns);
            //write out the TopK as tokens
            ArrayList<Integer> closestCenters = this.getClosestCenters(distances, this.TopK);
            String output = "";
            for(int i =0;i<closestCenters.size();i++){
                output += " " + closestCenters.get(i);
            }
            writer.write(output.trim());
            writer.newLine();writer.flush();
        }
        writer.close();
        reader.close();
    }
    
    public void generateTokensStack5() throws FileNotFoundException, IOException{
        BufferedWriter writer = null;
        if(new File(this.TokensFile).exists()){
        	return;
        } else {
        	writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(this.TokensFile))); 
        }
        
    	BufferedReader reader = null;
        if(this.DataFile.endsWith(".gz")){
        	//this is a gzip file
        	reader = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(this.DataFile))));
        }else{
        	//this is just a normal text file
        	reader = new BufferedReader(new InputStreamReader(new FileInputStream(this.DataFile)));
        }
        String row="";
        ArrayList<String> arr = new ArrayList<String>();
        while((row=reader.readLine())!=null){
        	arr.add(row.trim());
        }
        ArrayList<String> stack5 = new ArrayList<String>();
        //convert strings into stack5
        int index = 0;
        while(index+4<arr.size()){
        	String stacked5 = arr.get(index)  + " " +
        					 arr.get(index+1) + " " +
        					 arr.get(index+2) + " " +
        					 arr.get(index+3) + " " +
        					 arr.get(index+4);
        	stack5.add(stacked5);
			index += 1;
        }
        index = 0;
        
        while(index<stack5.size()){
        	row = stack5.get(index);
			//System.out.println(row);
        	//convert to a double array
            double[] columns = this.string2Double(row.trim().split(FILE_DELIM));
            //System.out.println(columns.length+" "+this.FeatureCount);
            if(columns.length!=this.FeatureCount){
                System.err.println("Format error in input file lala");
                System.exit(1);
            }
            double[] distances = this.calcDistance(columns);
            //write out the TopK as tokens
            ArrayList<Integer> closestCenters = this.getClosestCenters(distances, this.TopK);
            String output = "";
            for(int i =0;i<closestCenters.size();i++){
                output += " " + closestCenters.get(i);
            }
            writer.write(output.trim());
            writer.newLine();writer.flush();
            index += 1;
        }
        writer.close();
        reader.close();
    }
    
    private ArrayList<Integer> getClosestCenters(double[] distance, int k){
        ArrayList<Integer> closestCenters = new ArrayList<Integer>();
        while(closestCenters.size()<k){
            int minIndex = 0;
            double minVal = distance[minIndex];
            double maxVal = 0;
            for(int i=0;i<distance.length;i++){
                if(distance[i]<=minVal){
                    minVal = distance[i];
                    minIndex = i;
                }
                if(distance[i]>maxVal){
                    maxVal = distance[i];
                }
            }
            distance[minIndex] = maxVal+1;
            closestCenters.add(minIndex);
        }
        return closestCenters;
    }
    
    //a utility function to convert a string array to double
    private double[] string2Double(String[] vector){
        double[] points = new double[vector.length];
        int index = 0;
        while(index<vector.length){
            points[index] = Double.parseDouble(vector[index]);
            index++;
        }
        return points;
    }
    
    //this function will calculate the distance from the centers
    private double[] calcDistance(double[] point){
        double[] distance = new double[this.ClusterCount];
        for(int i=0;i<this.ClusterCount;i++){
            distance[i] = 0.0;
            int index = 0;
            while(index<point.length){
                //euclidean distance
                distance[i] += (point[index]-this.centerList.get(i)[index])*
                        (point[index]-this.centerList.get(i)[index]);
                index++;
            }
        }
        return distance;
    }
    
    public static void usage(){
        String message = "Usage: bof <KMeansCenters> <mfcc file> <TopK> <Output Directory>\n";
        System.out.println(message);
    }
}
