#include <vector>
#include <cmath>
#include <iostream>
using namespace std;
//gets point in between 2 others
struct valley {
	int start;
	int end;
	int bottom;
	int max; //max height of fill index
	int lside; //same as start but can't change 
	int rside; //same as end but can't change
	long double fillh;
	long double fillvol;
	long double fillmax;
};
struct point {
	long double x;
	long double y;
	long double fill;
	point();
	point(long double,long double);
};
point::point(){
}
point::point(long double a,long double b) {
	x = a;
	y = b;
	fill = 0;
}
point between(point a,point b,double height) { //assume b is higher/farther
	if (height == a.y)
		return a;
	point ret;
	ret.x = a.x+(b.x-a.x)*(height-a.y)/(b.y - a.y);
	ret.y = height;
	return ret;
}
point fetchpoint(int &iter,vector<point> &points,int side,int move,long double high,long double &realval){
	if (side == -1) { //moving left
		if (move == 0) { //if the descender
			if (iter == -1) { //if on edge
				realval = high;
				return point(points[0].x,high);
			}
			else {
				realval = points[iter].y;
				return between(points[iter+1],points[iter],high);
			}
		}
		else if (iter >= 0) { //if the ascender
			realval = points[iter].y;
			iter += move;
			return points[iter-move];
		}
	}
	else if (side == 1) { //moving right
		if (move == 0) { //if the descender
			if (iter == (int)points.size()) {
				realval = high;
				return point(points[points.size()-1].x,high);
			}
			else {
				realval = points[iter].y;
				return between(points[iter-1],points[iter],high);
			}
		}
		else if (iter <= ((int)points.size()-1)) { //if the ascender
			realval = points[iter].y;
			iter += move;
			return points[iter-move];
		}
	}
	return point(0,0); //dun fucked up
}
double Trapsum(vector<point> &points,int &start,int &end,int max) {
	long double sum = 0;
	//cout << "points: " << start << " " << end << endl;
	long double maxv = points[max].y;
	int a=0; //ensures correct iteration, -1 means left is off, 1 means right is off
	if ((max == end) && (start!=0)) { //crop to only appropriate region
		int i;
		for (i=end;i>start;i--)
			if ((points[i-1].y<=maxv) && (points[i].y>=maxv))
				break;
		end = i;
		a=-1;
		point temp = between(points[start+1],points[start],maxv);
		sum+=(points[start+1].y/2)*(points[start+1].x-temp.x);
	}
	else if ((max == start)&& (end!=(int)(points.size()-1))) { //same
		int i;
		for (i=start;i<end;i++)
			if ((points[i+1].y<=maxv) && (points[i].y>=maxv))
				break;
		start = i;
		a = 1;
		point temp = between(points[end-1],points[end],maxv);
		sum+=(points[end-1].y/2)*(temp.x-points[end-1].x);
	}
	for (int i=(start+(a==-1));i<(end-(a==1));i++) {
		sum+=(maxv-(points[i].y+points[i+1].y)/2.0)*(points[i+1].x-points[i].x);
	} //Trapezoidal sum YAY
	return sum;
}
int main() {
	vector<long double> heights;
	int fillheight = 5; //units per unit
	vector<point> points;
	points.push_back(point(0,5)); //test points initialization
	points.push_back(point(5,0));
	points.push_back(point(15,10)); //should for a like... check.
	points[0].fill = (points[1].x-points[0].x)*fillheight;
	bool running = true;
	for (int arf=0;(arf<20)&&running;arf++){
	//while (running) {
		running = false;
		vector<valley> vallies; //all vallies
		int a = points[0].y;
		int b = points[1].y;
		int c;
		int lEnd = 0;
		long double collectedfill = points[0].fill;
		bool ascending = false; //start by descending
		if (a<b) {
			ascending = true;
			vallies.push_back(valley());
			vallies[0].start = 0;
			vallies[0].bottom = 0;
		}
		for (int i=2;i<(int)points.size();i++) {
			c = points[i].y;
			points[i-1].fill = (points[i].x-points[i-1].x)*fillheight; //add fills
			if ((b<c)&&!ascending) { // if valley
				ascending = true;
				vallies.push_back(valley());
				vallies[vallies.size()-1].start = lEnd;
				vallies[vallies.size()-1].bottom = i-1;
			}
			if ((b>c)&&ascending) { // if hill
				ascending = false;
				lEnd = i;
				vallies[vallies.size()-1].end = lEnd;
				vallies[vallies.size()-1].fillvol = collectedfill;
				collectedfill=0;
			}
			collectedfill+=points[i-1].fill;
			a=b; //snake along
			b=c;
		}
		if (ascending == false) { //if a valley at end
			vallies.push_back(valley());
			vallies[vallies.size()-1].start = lEnd;
			vallies[vallies.size()-1].bottom = points.size()-1;
			vallies[vallies.size()-1].end = points.size()-1;
		}
		else {
			vallies[vallies.size()-1].end = points.size()-1;
		}
		vallies[vallies.size()-1].fillvol = collectedfill+points[points.size()-1].fill;//add fill
		for (int i=0;i<(int)vallies.size();i++) {
			if ((vallies[i].start == 0) && (vallies[i].end == (int)(vallies.size()-1)))
				vallies[i].max = (points[vallies[i].end].y>points[vallies[i].start].y)?vallies[i].end:vallies[i].start;
			if (vallies[i].start == 0)
				vallies[i].max = vallies[i].end;
			else if (vallies[i].end == (int)(vallies.size()-1))
				vallies[i].max = vallies[i].start;
			else
				vallies[i].max = min(points[vallies[i].start].y,points[vallies[i].end].y);
			vallies[i].lside = vallies[i].start; //set static members
			vallies[i].rside = vallies[i].end;
			vallies[i].fillmax = Trapsum(points,vallies[i].start,vallies[i].end,vallies[i].max);
		}
		//cout << vallies.size()<<endl;
		bool changed = true; //loop while things keep moving around
		while (changed) {
			changed = false;
			for (int i=0;i<(int)vallies.size();i++) { //distribute water (not necessary but makes this much more efficient)
				if (vallies[i].fillvol>vallies[i].fillmax) { //if enough water
					if (i>0) { //if there is a valley to the left
						if ((vallies[i-1].rside==vallies[i].max)&&((vallies[i-1].max!=vallies[i].max)||(vallies[i-1].fillvol<vallies[i-1].fillmax))) {//if eligible and not enough water or lower
							vallies[i-1].fillvol += (vallies[i].fillvol-vallies[i].fillmax);
							vallies[i].fillvol = vallies[i].fillmax; //transfers all extra water to next bin. shouldn't affect anything negatively and allows for more flow
							changed = true;
						}
					}
					else if (i<((int)vallies.size()-1)) { //if there is a valley to the right
						if ((vallies[i+1].lside==vallies[i].max)&&((vallies[i+1].max!=vallies[i].max)||(vallies[i+1].fillvol<vallies[i+1].fillmax))) {//if eligible and not enough water or lower
							vallies[i+1].fillvol += (vallies[i].fillvol-vallies[i].fillmax);
							vallies[i].fillvol = vallies[i].fillmax; //transfers all extra water to next bin. shouldn't affect anything negatively and allows for more flow
							changed = true;
						}
					}
				}
			}
		}
		for (int i=0;i<(int)points.size()-1;i++) { //for all points
			points[i].fill = 0; //reset fill
		}
		for (int i = 0;i<(int)vallies.size();i++) { //for all vallies (height adjuster)
			cout << vallies[i].fillvol << " " << vallies[i].fillmax<<endl;
			if (vallies[i].fillvol > vallies[i].fillmax) {
				running = true;
				//cout << vallies[i].fillvol << " " << vallies[i].fillmax<<endl;
				points[vallies[i].start+1].fill = vallies[i].fillvol-vallies[i].fillmax; //fill just member next to the start member with all the water because why not
				int a = 0; //-1 for left, 1 for right
				if (points[vallies[i].start].y!=points[vallies[i].end].y) {
					if ((vallies[i].max == vallies[i].end)&&(vallies[i].start!=0)) { //if max is end
						point temp = between(points[vallies[i].start+1],points[vallies[i].start],points[vallies[i].max].y);
						points[vallies[i].start+1].x = temp.x;
						points[vallies[i].start+1].y = temp.y; //level off the uneven point.
						a=-1;
					}
					else if((vallies[i].max == vallies[i].start)&&(vallies[i].end!=(int)(points.size()-1))){ //if max is start
						point temp = between(points[vallies[i].end-1],points[vallies[i].end],points[vallies[i].max].y);
						points[vallies[i].end-1].x = temp.x;
						points[vallies[i].end-1].y = temp.y;
						a=1;
					}
				}
				for (int j=vallies[i].start+(a==-1);j<vallies[i].end-(a==1);j++) {
					points[j].y = points[vallies[i].max].y; //raise the points to level
				}
			heights.push_back(points[vallies[i].max].y); //add to height table
			}
			else {
				points[vallies[i].start+1].fill = vallies[i].fillvol; //fill just member next to the start member with all the water because why not
			}
		}
		if (running == false) {
			for (int i=0;i<(int)vallies.size();i++) {
				int center = vallies[i].bottom,maxv = vallies[i].max;
				int rindex=center+1,lindex=center-1/*,oldrindex=rindex,oldlindex = lindex*/;
				long double lheight=points[lindex].y,rheight=points[rindex].y;
				long double lastsum = 0,sum=0,lastheight = points[center].y,lheightr=lheight,rheightr=rheight;
				point lastL=points[center],lastR=points[center],oldL=lastL,oldR=lastR;
				while (true) { //not even sure i need lheightr and rheightr
					lastsum = sum;
					if ((lindex==-1)&&(rindex==(int)points.size())) {
						long double diff = (vallies[i].fillvol-sum)/(points[rindex].x-points[lindex].x);
					  sum = vallies[i].fillvol;
						lastL = point(points[0].x,lastheight+diff);
						lastR = point(points[points.size()-1].x,lastheight+diff);
						rheight = lastR.y;
						lheight = lastL.y;
						break; //should not continue past here
					}
					else if ((lheightr>rheightr)||(lindex==maxv)||(lindex==0)) {//if left side is taller or maxed
						point templ = fetchpoint(lindex,points,-1,0,rheight,lheightr);//between(points[lindex+1],points[lindex],rheight);
						point tempr = fetchpoint(rindex,points,1,1,rheight,rheightr); //iter,points,side,move,high
						sum+=(lastR.x-lastL.x)*(rheight-lastheight); //add box
						sum+=(lastL.x-templ.x)*(rheight-lastheight)/2.0; //add triangle
						sum+=(tempr.x-lastR.x)*(rheight-lastheight)/2.0; // add triangle
						lastL = templ;
						lastR =	tempr;
						rheight = lastR.y;
					}
					else if ((lheightr<rheightr)||(rindex==maxv)||(rindex==((int)points.size()-1))) { //if right side is taller or maxed
						point templ = fetchpoint(lindex,points,-1,-1,lheight,lheightr);//between(points[lindex+1],points[lindex],rheight);
						point tempr = fetchpoint(rindex,points,1,0,lheight,rheightr); //iter,points,side,move,high
						sum+=(lastR.x-lastL.x)*(lheight-lastheight); //add box
						sum+=(lastL.x-templ.x)*(lheight-lastheight)/2.0; //add triangle
						sum+=(tempr.x-lastR.x)*(lheight-lastheight)/2.0; // add triangle
						lastL = templ;
						lastR =	tempr;
						lheight = lastL.y;
					}
					else if (lheightr == rheightr) {
						point templ = fetchpoint(lindex,points,-1,-1,lheight,lheightr);//between(points[lindex+1],points[lindex],rheight);
						point tempr = fetchpoint(rindex,points,1,1,lheight,rheightr); //iter,points,side,move,high
						sum+=(lastR.x-lastL.x)*(rheight-lastheight); //add box
						sum+=(lastL.x-templ.x)*(rheight-lastheight)/2.00000; //add triangle
						sum+=(tempr.x-lastR.x)*(rheight-lastheight)/2.00000; // add triangle
						lastL = templ;
						lastR =	tempr;
						rheight = lastR.y;
						lheight = lastL.y;
					}
					if (sum>=vallies[i].fillvol) {
						break;
					}
					if ((lastheight >= points[maxv].y)||(((lastL.x == oldL.x)&&(lastR.x==oldR.x))&&((lastL.y == oldL.y)&&(lastR.y==oldR.y)))) { //if clearly out of bounds
						break;
					}
					lastheight = lastR.y;
					oldR = lastR;
					oldL = lastL;
					//oldrindex = rindex;
					//oldlindex = lindex;
				} //end while
				long double Area = vallies[i].fillvol-lastsum,dtop = (lastR.x-lastL.x),dbottom=(oldR.x-oldL.x),dheight = (rheight-lastheight);
				long double dist = ((dbottom*dheight-sqrt(-2*dbottom*Area*dheight+2*dtop*Area*dheight+pow(dbottom,2)*pow(dheight,2)))/(dbottom-dtop)); //rawr (thanx mathematica)
				vallies[i].fillh = lastheight+dist;
				heights.push_back(vallies[i].fillh);
			} //end for
		} //end if
		for (int i=0;i<(int)points.size();i++) 
			cout << "; " << points[i].x << " " << points[i].y;
		cout << endl;
	}//end while
	long double largest = 0;

	for (int i=0;i<(int)heights.size();i++) {
		if (heights[i] > largest)
			largest = heights[i];
	}
	cout << largest << endl; 
}
