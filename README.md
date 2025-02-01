# Youtube Downloaded
Self explanatory name, just a program to download songs from youtube

## Usage
Run the binary, pass in directory to download and keep track of your downloaded songs as first command line argument and the path to the library as the second

### Library
The library is a markdown file where all the album are located.
The library should look like this:  
\# \{Artist} *Optional*  
\## \[{Album}]({Link to Album cover}) *Mandatory*  
\[{Song}]({Youtube link to song}) *Mandatory*  

## Dependencies
[curl](https://github.com/curl/curl)  
[FFmpeg](https://github.com/FFmpeg/FFmpeg)  
[yt-dlp](https://github.com/yt-dlp/yt-dlp)  