# How to set up a new C++ project in `Visual Studio 2022`

1. `Download and execute` the [Visual Studio 2022 Installer](https://visualstudio.microsoft.com/vs/community/) from the Microsoft website. Then select `Visual Studio Community 2022` and click on the `Install` button. If you `already installed` Visual Studio 2022, you can move on to `step 4`.

![Visual Studio Installer - Available](https://github.com/asm128/hdtree/assets/26621255/9ba87843-f8fa-4f70-ab8b-dbbc4c8e9efc)

2. You will be asked to select the packages you want to install. This tutorial is for C++ but you can select other packages if you like. The basic C++ package you should install is the one called `Desktop development with C++` (which is not necessarily for desktop only as I use it to develop for other platforms too).

![VS Installer - Packages](https://github.com/asm128/hdtree/assets/26621255/98171a10-7667-476e-a988-39f4cbcce5c1)

- 2.1 I recommend to also go to the `Individual components` tab and select the `Git extension` or `Git for Windows` that comes with it (until VS2019 you had both).

![Visual Studio Installer - Individual components](https://github.com/asm128/hdtree/assets/26621255/d7a5b714-09d7-48ef-b1a3-fdb32bcb7109)

3. When opening VS `for the first time`, a menu will let you choose `Visual C++` as your `default setting`. *

![Visual Studio 2017-19 - Import and export settings](https://github.com/asm128/hdtree/assets/26621255/29fb6ada-1730-4248-8d99-22f4c9b1b25c)

4. A `folder` will be needed where your development solutions will be placed. I called mine just `solutions`.

![C++ - dev folder](https://github.com/asm128/hdtree/assets/26621255/4be97e35-4cc5-4d69-9670-eb4467e4ada1)

5. You can then create a new solution and project as follows:

- 5.1 Open Visual Studio 2022 and click on `Create a new project`:

![VS2019 Get Started](https://github.com/asm128/hdtree/assets/26621255/96730941-675f-4f4f-a2d0-40a87e2ae122)

- 5.2 Select `Windows Desktop Wizard` as project template, then click on `Next`. 

![VS2019 Create a new project](https://github.com/asm128/hdtree/assets/26621255/28183e10-1912-4db6-9dc1-8e8187ecdc27)

- 5.3 Then type the Project name (such as `MyFirstProject`) and the name of the folder in the `Location` box, then click on `Create`. 

![VS2019 Configure your new project](https://github.com/asm128/hdtree/assets/26621255/308883bb-630a-4d71-ac47-db5fb94d53a9)

- 5.4 In the window that pops-up next, select `Console Application` as the `Application type` and check the `Empty Project` checkbox.

![VS2017-19 Windows Desktop Wizard](https://github.com/asm128/hdtree/assets/26621255/b4abad12-5380-4d5b-8eea-4bc2373b0422)

- 5.5 Click on `OK` and the project and project files will be created automatically as shown below.

![C++ - Solution Folder](https://github.com/asm128/hdtree/assets/26621255/109aae71-cc8c-4f1f-ad9b-c1cdc84b0bdc)

6. After setting up the project, you will be able to add files to it from the samples you find here or browsing the web. If you want to `test` some code you can write a `hello world example` as follows:

- 6.1 Right click on the `project name` in the `Solution Explorer` in `Visual Studio` (at the right or left of the edit area) and in the pop-up menu select `Add` -> `New item`

![VS2017-19 - Add, New item](https://github.com/asm128/hdtree/assets/26621255/cd232830-5638-4277-86c9-d57de8215c39)

- 6.2 Select `Visual C++` -> `C++ file (.cpp)` and type the name of the file, such as `main.cpp` at the name box. Then Click on `Add` to create the file.

![VS2017-19 - Add New Item Window](https://github.com/asm128/hdtree/assets/26621255/bbfba0fe-748e-4203-9d9e-fe336c5b6b2b)

- 6.3 Type or copy the following code into the `main.cpp` file:

```cpp
#include <stdio.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
```

![VS2017-19 - Source code](https://github.com/asm128/hdtree/assets/26621255/056e2787-1569-4f70-b16c-e2bb80449902)

- 6.4 Press `F7` (or `ctrl`+`shift`+`b` if you didn't set up Visual C++ settings) to build the executable and `F5` to execute in debug mode. 

![VS2017-19 - Hello World](https://github.com/asm128/hdtree/assets/26621255/0e3d8b0b-7d55-451a-84a8-a520685f68a7)

### If you didn't set up `Visual C++ settings` during install you can do it later by going to menu `Tools` -> `Import and export settings` -> `Reset all settings` and the option will appear again. The screenshot shown is from that window because I had no fresh install of Visual Studio to see the window appear during the first run. 

![Visual Studio 2017-19 - Import and export settings](https://github.com/asm128/hdtree/assets/26621255/9edc33bc-d1a1-45f6-b9f7-1e2dc05e65e0)