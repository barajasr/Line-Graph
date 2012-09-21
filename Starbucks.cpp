//-------------------------------------------------------------------------------------------------------
// Starbucks.cpp
// Richard Barajas
// barajasr@mail.fresnostate.edu
//
// compile on linux:
// 	g++ -o counter Starbucks.cpp -lsfml-graphics -lsfml-window -lsfml-system
//
//------------------------------------------------------------------------------------------------------
// This program is used to keep track of the number of customer that purchase items from Starbucks.
// For each person that leaves the establishment with some item(s), that person is counted by pressing
// the up button. In case of counting to many at a time one could subtract by pressing the down button.
// The number of customers is represented in two manners: a real time counter found on the top left of
// the screen and a graph. The graph show the number of customers that are recorded for every period of 
// time set in the CountMoniter class. The graphs y-axis show the number of new customers, not total,
// for every period of recordTime. The y-axis only goes up to 30, so don't increment by too much 
// in between graph updates. I record data every five minutes, but modified recordTime to be 
// set to 10 seconds for this sample. As such graph will update every 10 seconds. Program is exited by 
//pressing the close button.
//
//------------------------------------------------------------------------------------------------------
#include<SFML/Graphics.hpp>
#include<ctime>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>

class Log{
	private:
		std::ofstream file; 					// file to log data captured
		unsigned lastValueLogged; 				// used in determining new customers
	protected:
		std::vector<unsigned> peoplePerTimeSlice; 	// logs new customer totals for a given time period
	public:
		Log():lastValueLogged(0){
			peoplePerTimeSlice.push_back(0);
		}
		// at exit log all data record
		~Log(){
			file.open("Count.log", std::ios::app);
			if(file.is_open()){
				unsigned total(0);
				for(unsigned i(0); i < peoplePerTimeSlice.size(); i++){
					file << peoplePerTimeSlice.at(i) << " ";
					total += peoplePerTimeSlice.at(i);
				}
				file << total << "\n";
			}else{
				std::cerr << "Was not able to record data to file for logging!!\n\n";
			}
		}
		// add new customers to vector record
		void addTimeSliceData(const sf::Sprite &ones,const sf::Sprite &tens, const sf::Sprite &hundreds,const sf::Sprite &thousands){
			sf::IntRect rect = thousands.GetSubRect();
			unsigned Count = (rect.Left / 28) * 1000;
			rect = hundreds.GetSubRect();
			Count += (rect.Left / 28) * 100;
			rect = tens.GetSubRect();
			Count += (rect.Left / 28) * 10;
			rect = ones.GetSubRect();
			Count += (rect.Left / 28);

			peoplePerTimeSlice.push_back(Count - lastValueLogged);
			lastValueLogged = Count;
		}
};

class Graph{
	protected:
		sf::Shape 	timeAxis,
					peopleAxis;
		std::vector<sf::Shape> 	graph, 				// line graph representing Starbucks customers
								fiveMinMarker, 		// x axis scale will change periodically
								fivePersonMarker; 	// 1 person : 5 pixels
	public:
		Graph(){
			peopleAxis = sf::Shape::Line(30.0f, 50.0f, 30.0f, 200.0f, 1.0f, sf::Color(255, 0, 0));
			timeAxis = sf::Shape::Line(30.0f, 200.0f, 250.0f, 200.0f, 1.0f, sf::Color(255, 0, 0));
			
			for(unsigned i(0); i < 5; i++)
				fivePersonMarker.push_back( sf::Shape::Line(25.f , 75.0f + (25.0f * i), 250.0f, (75.0f + 25.0f * i), 1.0f, sf::Color(0, 150, 0)));
		}
		void updateGraph(){}
};

class CountMonitor: public Log, public Graph{
	private:
		bool error;
		float recordTime;  				// time recording snapshots of customers
		sf::Image Image; 				// sprite array to have in memory
		sf::RenderWindow App; 			// this is a reference to screen
		sf::Sprite 	Ones, 				// Sprites that will represent the counter
					Tens,
					Hundreds,
					Thousands;
		// act like ripple subtraction, uses sprite index to identify value 
		// same holds true for the other decrement functions
		void decrement(){
			sf::IntRect rect = Ones.GetSubRect();
			rect.Offset(-28, 0); 						// move index by one left
			Ones.SetSubRect(rect); 						// set sprite to new location
			if(rect.Left < 0){ 							// if sprite == negative index, adjust 
				Ones.SetSubRect(sf::IntRect(252, 0, 280, 28)); 	// set index to 9th index
				decrementTens(); 								// subtract one from Tens place
			}
		}
		void decrementHundreds(){
			sf::IntRect rect = Hundreds.GetSubRect();
			rect.Offset(-28, 0);
			Hundreds.SetSubRect(rect);
			if(rect.Left < 0){
				Hundreds.SetSubRect(sf::IntRect(252, 0, 280, 28));
				decrementThousands();
			}
		}
		void decrementTens(){
			sf::IntRect rect = Tens.GetSubRect();
			rect.Offset(-28, 0);
			Tens.SetSubRect(rect);
			if(rect.Left < 0){
				Tens.SetSubRect(sf::IntRect(252, 0, 280, 28));
				decrementHundreds();
			}
		}
		void decrementThousands(){
			sf::IntRect rect = Thousands.GetSubRect();
			rect.Offset(-28, 0);
			Thousands.SetSubRect(rect);
			if(rect.Left < 0)
				Thousands.SetSubRect(sf::IntRect(252, 0, 280, 28));
		}
		// Acts like ripple addition,uses sprite index to identify value
		// same holds true for the other increment functions
		void increment(){
			sf::IntRect rect = Ones.GetSubRect();
			rect.Offset(28, 0); 						// move index by one right, each sprite is 28 x 28
			Ones.SetSubRect(rect); 						// set sprite to new location
			if(rect.Left >= 280){ 						// if sprite is out of range, adjust
				Ones.SetSubRect(sf::IntRect(0, 0, 28, 28)); 	// set index to 0th index
				incrementTens(); 								// add by one in the Tens place
			}
		}
		void incrementHundreds(){
			sf::IntRect rect = Hundreds.GetSubRect();
			rect.Offset(28, 0);
			Hundreds.SetSubRect(rect);
			if(rect.Left >= 280){
				Hundreds.SetSubRect(sf::IntRect(0, 0, 28, 28));
				incrementThousands();
			}
		}
		void incrementTens(){
			sf::IntRect rect = Tens.GetSubRect();
			rect.Offset(28, 0);
			Tens.SetSubRect(rect);
			if(rect.Left >= 280){
				Tens.SetSubRect(sf::IntRect(0, 0, 28, 28));
				incrementHundreds();
			}
		}
		void incrementThousands(){
			sf::IntRect rect = Thousands.GetSubRect();
			rect.Offset(28, 0);
			Thousands.SetSubRect(rect);
			if(rect.Left >= 280)
				Thousands.SetSubRect(sf::IntRect(0, 0, 28, 28));
		}
		
	public:
		CountMonitor():App(sf::VideoMode(280, 225), "Counter", sf::Style::Close), error(false), recordTime(10.0f){
			if(Image.LoadFromFile("numbers.png")){ 				// Attempt to load image to memory
				sf::Color colorKey(191, 205, 205); 				// only show the number
				Image.CreateMaskFromColor(colorKey);

				// Set the sprites to the number zero and move them to their screen location
				Ones.SetImage(Image);
				Ones.SetPosition(84.0f, 0.0f);
				Ones.SetSubRect(sf::IntRect(0, 0, 28, 28));
				Tens.SetImage(Image);
				Tens.SetPosition(56.0f, 0.0f);
				Tens.SetSubRect(sf::IntRect(0, 0, 28, 28));
				Hundreds.SetImage(Image);
				Hundreds.SetPosition(28.0f, 0.0f);
				Hundreds.SetSubRect(sf::IntRect(0, 0, 28, 28));
				Thousands.SetImage(Image);
				Thousands.SetSubRect(sf::IntRect(0, 0, 28, 28)); 
			}else{ error=false;} 
		}
		// Handle input and increment or decrement accordingly, 
		// showing the changes on screen while logging data every 5 minutes
		int runApp(){
			unsigned i(0); 			// just to prevent unsigned subtraction when zero
			if(!error){
				sf::Clock clock;
				while(App.IsOpened()){
					sf::Event Event;
					while(App.GetEvent(Event)){
						if(Event.Type == sf::Event::Closed)
							App.Close();
						if(Event.Type == sf::Event::KeyPressed){
							if(Event.Key.Code == sf::Key::Up){
								increment();
								i++;
							}else if(Event.Key.Code == sf::Key::Down){
								if(i > 0){
									decrement();
									i--;
								}
							}
						}
					}
					// Log data every 300 seconds or 5 minutes
					// Changed to 10 seconds in sample for classmates
					if(clock.GetElapsedTime() >= recordTime){
						addTimeSliceData(Ones, Tens, Hundreds, Thousands);
						updateGraph();
						clock.Reset();
					}
					draw();
					sf::Sleep(0.01f); 			// Reduce CPU usage with timeout
				}
				return EXIT_SUCCESS;
			}
			return EXIT_FAILURE;
		}
		// Draw all sprites to screen
		void draw(){
			// Blit sprites to screen 
			App.Clear();
			App.Draw(Thousands);
			App.Draw(Hundreds);
			App.Draw(Tens);
			App.Draw(Ones);
	
			// Blit lines that belong to graph portion of screen
			App.Draw(peopleAxis);
			App.Draw(timeAxis);
			for(unsigned i(0); i < fivePersonMarker.size(); i++)
				App.Draw(fivePersonMarker.at(i));
			for(unsigned i(0); i < fiveMinMarker.size(); i++)
				App.Draw(fiveMinMarker.at(i));
			for(unsigned i(0); i < graph.size(); i++)
				App.Draw(graph.at(i));
			
			// Now show update to screen 
			App.Display();
		}
		void updateGraph(){
			unsigned timeSlices = peoplePerTimeSlice.size();
			if(timeSlices > 0){
				unsigned 	xSpacing = 220 / timeSlices,
							xCurr = 30 + xSpacing,
							xPrevious(30), 
							yPrevious(200);
				for(unsigned i(0); i < graph.size(); i++){
					unsigned yCurr = 200 - (5 * peoplePerTimeSlice.at(i + 1));
					graph[i] = sf::Shape::Line(xPrevious, yPrevious, xCurr, yCurr, 1.0f, sf::Color(255, 255, 255));
					fiveMinMarker[i] = sf::Shape::Line(xCurr , 50.0f, xCurr , 205.0f, 1.0f, sf::Color(0, 150, 0));
					xPrevious = xCurr;
					yPrevious = yCurr;
					xCurr += xSpacing;
				}		
				graph.push_back(sf::Shape::Line(xPrevious, yPrevious,xCurr, 
												200 - (5 * peoplePerTimeSlice.at(peoplePerTimeSlice.size() -1 )),
												1.0f, sf::Color(255, 255, 255)));
				fiveMinMarker.push_back(sf::Shape::Line(xCurr , 50.0f, xCurr , 205.0f, 1.0f, sf::Color(0, 150, 0)));
			}

		}
};

int main(int argc, char *argv[]){
	CountMonitor starbucksFuckers;
	return starbucksFuckers.runApp();
}
