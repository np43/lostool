# Build Instructions

## Dependencies

* qt5-default
* liblas-c-dev
* libcgal-dev

## Steps To Build
```
mkdir build && cd build
cmake ../
make
```

# Usage

## Configuration
The following configuration options need to be specified before the tool is run:
1. `lasdir`

    The path to the directory containing the point cloud data

2. `folderprefix`

    The point cloud data is assumed to be present in numbered sets. For example, SET-1, SET-2, SET-3. The folder prefix
    is the part of this folder name without the number. For the given example, the prefix is 'SET-'

3. `folderStart`

    The starting number of the set

4. `folderEnd`

    The last number of the set

5. `polesCloudFile`

    The name of the point cloud file which corresponds to the street light poles. Example: streetlightpoles.las

6. `polesFile`

    Specify a file name to store the information about the identified poles.

7. `netFile`

    Specify a file name to store the network information.

8. `linksFile`

    Specify a file name to store the links in the final network.

9. `ignorelas`

    A list of point cloud files to ignore. Specify just the file name.

10. `max_dist`

    The maximum allowed distance for a link (in metres)

11. `min_dist`

    The minimum allowed distance for a link (in metres)

12. `ap_range`

    The range of the access points (in metres)

For example, if the directory structure is as follows,

    home
     |  user
     |   |  data  
     |   |   |  SET-1
     |   |   |  SET-2
     |   |   |  SET-3
     |   |   |  SET-4

and the configuration is as follows,

    `lasdir`: "/home/user/data"
    `folderprefix`: "SET-"
    `folderStart`: 1
    `folderEnd`: 4

then the following folders are assumed to have the point cloud data (.las files),

    /home/user/data/SET-1
    /home/user/data/SET-2
    /home/user/data/SET-3
    /home/user/data/SET-4

## Find Poles and Check Links

Once the build is done, two executables are created:

    1. checklinks
    2. net2d

1. `checklinks` must be run first to find poles and check all the possible links between them.

    ![alt text](checklinks.png)

    ![alt text](checklinksdone.png)

    Once the poles are found and the links are checked, two file are created: `poles.txt` and `network.txt`.


    `poles.txt` This file contains the list of poles (UTM coordinates) found from the point cloud data. The following
    is the format of the file:

        Line1: Offset coordinates

        Line2: Coordinates of bounding box (minx, miny, minz) and (maxx, maxy. maxz)

        Line3: No .of poles

        Remaining lines: list of coordinates of poles (bottom, top) one pole per line

    `network.txt` This file contains the information about the pairwise links between poles. Each pair is represented
    as follows:

        `[polenumber1] [polenumber2] [distance] [nsublinks]`

        `nsublinks` is the number of possible sublinks between the two poles. If a link is possible for a pair of poles,
        then the `distance` and `nsublinks` are both positive and the next `nsublinks` lines contain the coordinates
        of these sublinks (start, end) on the respective poles. If a link is not possible either because the poles are too far/close
        or there are no possible sublinks, then the distance is `-1`.
2. Next, `net2d` may be run to form the network.

    ![alt text](net2d.png)

    The textbox on the left of the main window can be used to enter the numbers of the poles to connect.
    Each line takes two pole numbers separated by a comma. So the list of links in the box basically corresponds
    to the network formed thus far. These pole numbers may be chosen to be the access points.

    ![alt text](net2dlinks.png)
