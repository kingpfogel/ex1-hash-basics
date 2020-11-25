# Übung 1
Solutions to the first exercise sheet of experimental algorithms. (Group: L. Thyssen and N. Neesemann)

### Overview:
- tool: simexpal version 0.4
- run condition: must run on gruenau
- test environment: gruenau3 or gruenau4

### Common workflow:

- git clone git@github.com:kingpfogel/ex1-hash-basics.git
- git checkout -b PAK_your-feature-branch-name
- Change somethings on that branch and add and commit these changes
- git add --all / git add some-file.py
- git commit -m 'meaningful message'
- git push origin PAK_your-feature-branch-name
- Open a pull request on github.com
sdfasdf

# Documentation / Notes

### Basic Setup

-

install simexpal, meson package in [pycharm](https://www.jetbrains.com/help/pycharm/installing-uninstalling-and-upgrading-packages.html)

-

### Connect to server:

```ssh -l thyssenl@hu-berlin.de gruenau3.informatik.hu-berlin.de```

```
#doesnt work
#export PATH=$PATH:$HOME/.local/bin
setenv PATH ${PATH}:"$HOME/.local/bin"
echo $PATH  
pip3 install meson --user
pip3 install simexpal --user
cd EA/ex1-hash-basics
git checkout 2af56c5bfa7f5622a979535abdbd481cc521cff6
git pull origin master
simex develop hashing
simex e launch
``` 
wait 15 min 
open ssh connection in new terminal window to check progress:
 
```
simex e 
```
when finished:

```
simex archive

```
copy to local machine: 

```
ssh -l thyssenl@hu-berlin.de gruenau3.informatik.hu-berlin.de 'cat /home/wiwi/thyssenl/EA/ex1-hash-basics/output/hashing~chaining,ff50@_dev/dummy.out' > /Users/leonhardthyssen/Desktop/test.txt

```
- [HU Rechner Pool](https://www.informatik.hu-berlin.de/de/org/rechnerbetriebsgruppe/rooms/Zugaengliche-rechner-institut)
- [download from server ](https://stackoverflow.com/questions/9427553/how-to-download-a-file-from-server-using-ssh)
- You also have to add $HOME/.local/bin to your PATH: [set csh path var](https://stackoverflow.com/questions/40968061/bad-modifier-in)
- On gruenau, you have to pass --user to pip3 to install Python packages into your home directory.
- [fix meson location](https://github.com/mesonbuild/meson/issues/1613) / Make sure to set the correct PATH variable

[meson docu](https://mesonbuild.com/Quick-guide.html)

[simexpal docu ](https://simexpal.readthedocs.io/en/latest/quick_start.html)

[Github repo](https://github.com/kingpfogel/ex1-hash-basicshttps://github.com/kingpfogel/ex1-hash-basics) 



check python packages: ``pip3 list``

start python command line: ``python``

end python command line: ``ctrl + D``


##Jupyter Notebook

[Tutorial](https://www.dataquest.io/blog/jupyter-notebook-tutorial/)

[Tutorial matplot](https://www.datacamp.com/community/tutorials/matplotlib-tutorial-python)


run jupyter notebook:

```
brew install libyaml
pip3 install matplotlib
pip3 install pyyaml
jupyter notebook
print('Hello World!')
```
shortcuts: 

```
option + Enter: execute code in cell
dd: delete selected cell
```

