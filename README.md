# Youtube Downloaded
Self explanatory name, just a program to download songs from youtube

## Usage
Run the binary, pass in directory to download and keep track of your downloaded songs as first command line argument and the path to the [library](###Library) file as the second,

### Library
The library is a markdown file where all the album are located.
The library should look like this:  
\# \{Artist} *Optional*  
\## \[{Album}]({Link to Album cover}) *Mandatory*  
\[{Song}]({Youtube link to song}) *Mandatory*  
.  
.  
.  

### Principle
The program loads all the files from the download directory and all the albums from the library file and compares the two, if it finds a downloaded file that doesn't exist in the library it will ask to delete it, if it finds a song in the library that is not downloaded it will [download](####Download) it,
#### Download
The program will download the link to the album cover in to the download directory as temp.jpg, it will then download all the songs of the album one by one as temp.mp3 and then using [FFmpeg](https://github.com/FFmpeg/FFmpeg) it will create an mp3 flie with the name of the song, it will have the thumbnail you provided next to the album name and all the metadata (year, Artist and correct name of the album) of the song as it got from [musicbrainz](https://musicbrainz.org/). 

## Dependencies
[curl](https://github.com/curl/curl)  
[FFmpeg](https://github.com/FFmpeg/FFmpeg)  
[yt-dlp](https://github.com/yt-dlp/yt-dlp)  


# TODO:
Remove unneeded files
