# Crime And Dining
![GIF of basic interaction](http://dijitalelefan.com/images/github/PizzaPreview.gif)


An analysis of the correlation between crime and dining in the city of Boston
(See the full demo at [http://dijitalelefan.com/crimeAndDining](http://dijitalelefan.com/crimeAndDining))

-----
Using data from the city of Boston on [crime incidents](https://data.cityofboston.gov/Public-Safety/Crime-Incident-Reports/7cdf-6fgx) and [active food establishment licenses](https://data.cityofboston.gov/Permitting/Active-Food-Establishment-Licenses/gb6y-34cq), I investigated the corelation between occurences of crime and places to eat. To do so, for each licensed restauraunt in Boston, I compiled all of the crime incidents that took place within 100 meters of the location, took into account how long ago the crime occured, the type of crime, and any weapons involved, and I calculated a heuristitc for how "dangerous" the crime was relative to the restauraunt. The Danger Rating was then the sum of the dangers of each individual crime.

This analysis seemed cool, but it was really just a bunch of boring numbers and the overall trends were hard to notice. So I built [this!](http://dijitalelefan.com/crimeAndDining) A display of all this data on top of Google Maps, with a heatmap showing density of crime throughout the city, and markers for all the restauraunts that can be expanded to show relevent information for each one. Additionally, you can filter the results to only display restauraunts with, say, "Pizza" in the name, or perhaps to look only at Starbucks or another chain, to see the general trends throughout the city. 

I don't make much of a claim of scientific accuracy or reason, so these numbers are not necessarily terribly indicative of the danger of eating at a location. However, if there have been 400 crimes around a place I ate last week in the past few years, that is something that I don't mind knowing.

##How it all works

Originally, I wrote all of the analysis code in python over the span of an hour or two, and it was nice and consice and I let it run for a while... and when I checked back the next day, it was still running. Consequently, I rewrote it in C++, which is the language I'm most comfortable with optimizing in. In the end, this became the basic flow for running the analysis:

1. Download the Active Food Establishment Licenses and Crime Incident Reports databases from the city of Boston, and put them in the data folder. An older versoin of the databases ar already there.
2. Not all of the restauraunts in the databse have stored latitude/longitude coordinates that is necessary for this analysis, so run the python file locationFinder.py. This uses Google's Geocoding API and the addresses of the restauraunts to determine their geographical location, and requires an API key (I stored mine in a file config.py that has not been uploaded to GitHub). It will output a json file with information on the location to data/locs.json.
3. Compilethe C++ code with '''g++ -g -std=c++11 -o analyze *.cpp''' and run it. The analysis is done!
4. Although, maybe not, here is a caveat: I stored the type of crime as an integer, as there are less then 100 distinct incident types recorded in the Crime data. As this was basically just a one time thing for me, the integer merely refers to the order in which a specific incident type showed up in the crime file; therefore, if you change the crime file or download a new one, it will probably change the integer refering to the type. This is almost inconsequential, as I mostly ignore the type, but: MedAssist is a very common incident type whose name sounds very innocuous, so I wanted to ignore it. Using the crime data currently in data, the incident MedAssist is assigned the integer 10, and so consequentially I defined the term '''MED\_ASSIST''' in Crime.h as 10. This means that MedAssists will be ignored in my code. If you change the crime file, simply run the analysis and the integer refering to MedAssist will be outputted at the end; change the '''MED\_ASSIST''' constant to this and recompile and rerun, and everything will go swimingly.
5. Finally, I uploaded the outputted data on crimes and restauraunts to 2 Google Fusion Tables and used that to intgreate with the Google Maps API to create the web app stored within the site directory and [visible here](http://dijitalelefan.com/crimeAndDining) (all of these links point to the same place).

Really though, this code was mostly just a one-off thing to run to generate the data for the web app. I could create a cron job to update the data on, say, a weekly basis (by querying the Boston data API and by updating the Fusion Tablse through that API), but that would take a bit more time and I'm relatively busy with school work. The analysis process was just a process and is ultimately not as interesting as the results.

###C++ Code Internals

A breif explanation for how the C++ code works. It is divided up into several files:
* Crime.h and Crime.cpp - These files describe the struct Crime (containing information on a specific crime) and functions to calculate the danger of that crime relative to itself and relative to a restauraunt
* Restauraunt.h and Restauraunt.cpp - These files describe the class Restauraunt, which reads, stores, and outputs all of the data associated with an individual restauraunt
* Location.h and Location.cpp - These files describe my simplistic Location class, storing 2 doubles representign a coordinate, and a few associated functions
* Date.h and Date.cpp - These files describe my super simplistic Date class, storing simply the month, day, and year, and approximating differences between dates
* QuadTree.hpp and QuadTree.tpp - These files describe my QuadTree template class, which I am pretty certain is a quad tree? I have never worked with that data structure before, but basically it was so I could store restauraunts in a structure that would quickly allow me to find all restauraunts within a certain radius given (crime's) location
* analysis.cpp - This is the main file, with the main function. It reads in the locs.json file, reads in the restauraunts and crimes, calculates the crime cost per restauraunt, and ouputs everything agin.


For more information, if you feel up to it, you can consult the comments in the code.
