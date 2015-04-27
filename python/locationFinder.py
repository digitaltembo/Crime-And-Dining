################################################################################
# locationFinder.py                                                            #
#                                                                              #
# Nolan Hawkins, April 2015                                                    #
#                                                                              #
# Originally, the entire analysis was done in python, but that proved too      #
# inefficient so I reimplemented most bits in C++. However, the requests       #
# library in python was so simple that I just kept this bit; after all, it     #
# only needs to be called once per version of the                              #
# Active_Food_Establishment_Licenses file.                                     #
#                                                                              #
# And now might be a good time to indicate what it actually does: this program #
# reads the food licenses file, finds any restauraunts with unspecified        #
# coordinates, and uses Google's Geocoding API to guess the coordinates from   #
# the address, which does seem to be given 100% of the time. Then it outputs   #
# this data in a JSON file with keys of addresses and values of locations      #
################################################################################

import requests, csv, time, config


IN_FILE  = '../data/Active_Food_Establishment_Licenses.csv'
OUT_FILE = '../data/locs.json'

# Uses Google's Geocaching API to get location from address
# The limit to the API is 5 calls per second, so it is necessary to sleep 5
# seconds between calls
def getLocationFromAddress(address):
    print 'parsingAddress:',address
    url = 'https://maps.googleapis.com/maps/api/geocode/json'
    data = {'key':config.API_KEY, 'address':address}
    r = requests.get(url, params=data)
    addressData = r.json()
    # Ensure api isn't being called too frequently
    time.sleep(0.2)
    if(addressData['status']=='OK'):
        try:
            addressData = addressData['results'][0]['geometry']['location']
        except:
            return (0,0)
        return (addressData['lat'], addressData['lng'])
    return (0,0)

def parseAddress(row):
    return  row['Address'] +' '+row['City']+', '+row['State'] + ', '+row['Zip']

foodFile  = open(IN_FILE)

foodReader = csv.DictReader(foodFile)
i=0
addresses={}
for row in foodReader:
    location = row['Location']
    # A location is unset iff the 2nd character is a 0
    if(location[1] == '0'):
        address = parseAddress(row)
        addresses[address] = getLocationFromAddress(address)
    if(i%100 == 0):
        print i,'restauraunts parsed'
    i+=1
 
with open(OUT_FILE, 'w') as outfile:
    json.dump(data, outfile)