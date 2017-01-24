
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
import java.util.Arrays;
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
        centerList = new ArrayList();
        this.initCentroids(centroidFile);
        this.TopK = topK;
        this.DataFile = dataFile;
	String temp = (new File(dataFile)).getName();
        this.TokensFile = outPath+"/"+temp.substring(0,temp.lastIndexOf('.'))+FILE_TYPE;
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
        obj.generateTokens();
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
    private void generateTokens() throws FileNotFoundException, IOException{
		BufferedWriter writer = 
                new BufferedWriter(new OutputStreamWriter(new FileOutputStream(this.TokensFile)));
        BufferedReader reader = null;
		if(this.DataFile.endsWith(".gz")){
            reader = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(this.DataFile))));
        }else{
			//this is a text file
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
    
    private ArrayList<Integer> getClosestCenters(double[] distance, int k){
        ArrayList<Integer> closestCenters = new ArrayList();
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
