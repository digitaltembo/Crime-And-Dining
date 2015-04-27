# Crime And Dining
An analysis of the correlation between crime and dining in the city of Boston
(See the full demo at [http://dijitalelefan.com/crimeAndDining](http://dijitalelefan.com/crimeAndDining))

-----
Using data from the city of Boston on [crime incidents](https://data.cityofboston.gov/Public-Safety/Crime-Incident-Reports/7cdf-6fgx) and [active food establishment licenses](https://data.cityofboston.gov/Permitting/Active-Food-Establishment-Licenses/gb6y-34cq), I investigated the corelation between occurences of crime and places to eat. To do so, for each licensed restauraunt in Boston, I compiled all of the crime incidents that took place within 100 meters of the location, took into account how long ago the crime occured, the type of crime, and any weapons involved, and I calculated a heuristic for how "dangerous" the crime was relative to the restauraunt. The Danger Rating was then the sum of the dangers of each individual crime.

This analysis seemed cool, but it was really just a bunch of boring numbers and the overall trends were hard to notice. So I built [this!](http://dijitalelefan.com/crimeAndDining) A display of all this data on top of Google Maps, with a heatmap showing density of crime throughout the city, and markers for all the restauraunts that can be expanded to show relevent information for each one. Additionally, you can filter the results to only display restauraunts with, say, "Pizza" in the name, or perhaps to look only at Starbucks or another chain, to see the general trends throughout the city. 

I don't make much of a claim of scientific accuracy or reason, so these numbers are not necessarily terribly indicative of the danger of eating at a location. However, if there have been 400 crimes around a place I ate last week in the past few years, that is something that I don't mind knowing.

##How it all works

Todo
