
import sys, os, stat
from shutil import copyfile, copytree, rmtree
import glob
import subprocess
import random
import pandas as pd
import xml.etree.ElementTree as ET
import time

class NoMASS(object):

    def __init__(self):
        self.runLocation = ""
        self.simulationLocation = "simulation"
        self.locationOfNoMASS = ""
        self.NoMASSstr = "NoMASS"
        self.configurationDirectory = ""
        self.simulationFile = "SimulationConfig.xml"
        self.activityFile = "Activity.xml"
        self.largeApplianceFile = "AppliancesLarge.xml"
        self.PVFile = "PVBowler2013_365.csv"
        self.smallApplianceFolder = "SmallAppliances"
        self.resultsLocation = "Results"
        self.outFile = "NoMASS.out"
        self.appLearningFile = '*.dat'
        self.numberOfSimulations = 1
        self.learn = False
        self.learntData = ""
        self.clean = True
        self.PandasFiles = False
        self.seed = -1
        self.printInput = False
        self.epsilon = 0.1
        self.alpha = 0.1
        self.gamma = 0.1
        self.pddf = pd.DataFrame()
        self.start = time.time()

    def deleteLearningData(self):
        ll = self.learntDataLocation()
        if os.path.exists(ll):
            rmtree(ll)

    def printConfiguration(self):
        print "Run location: {}".format(self.runLocation)
        print "locationOfNoMASS: {}".format(self.locationOfNoMASS)
        print "configurationDirectory: {}".format(self.configurationDirectory)
        print "resultsLocation: {}".format(self.resultsLocation)
        print "printInput: {}".format(self.printInput)
        print "numberOfSimulations: {}".format(self.numberOfSimulations)
        print "Learning: {}".format(self.learn)
        con = self.configurationDirectory
        tree = ET.parse(con + self.simulationFile)
        root = tree.getroot()
        for buildings in root.findall('buildings'):
            for building in buildings.findall('building'):
                for agents in building.findall('agents'):
                    agentcount = 0;
                    for agent in agents.findall('agent'):
                        agentcount = agentcount +1
                    print "Number of agents {}".format(agentcount)
                for apps in building.findall('Appliances'):
                    print "Number of large appliances {}".format(len(apps.findall('Large')))
                    print "Number of small appliances {}".format(len(apps.findall('Small')))
                    print "Number of pv appliances {}".format(len(apps.findall('pv')))
                    if apps.findall('Grid'):
                        print "Grid enabled"

    def simulate(self):
        self.start = time.time()
        if self.printInput:
            self.printConfiguration()
        for x in range(0, self.numberOfSimulations):
            if x % 25 == 1:
                elapsed = time.time() - self.start
                print "Simulation: %i Time: %02d seconds"  % (x, elapsed)
            self.copyToRunLocation(x)
            self.makeExecutable(x)
            self.configuration(x)
            self.run(x)
            self.copyToResultsLocation(x)
            if self.clean:
                rmtree(self.runLoc(x))
        if self.PandasFiles:
            self.pddf.to_hdf(self.resultsLocation + 'NoMASS.out.hdf','NoMass',mode='w')

    def learning(self):
        return self.learn

    def configuration(self, x):
        rl = self.runLoc(x)
        tree = ET.parse(rl + self.simulationFile)
        root = tree.getroot()
        if self.seed < 0:
            random.seed()
        else:
            random.seed(self.seed)
        s = str(random.randint(1,99999))
        root.find('seed').text = s

        for buildings in root.findall('buildings'):
            for building in buildings.findall('building'):
                for apps in building.findall('Appliances'):
                    for ll in apps.findall('LargeLearning'):
                        if ll.find('updateQTable') is not None :
                            if self.learning():
                                ll.find('updateQTable').text = str(1)
                            else:
                                ll.find('updateQTable').text = str(0)
                        else:
                            newNode = ET.Element('updateQTable')
                            newNode.text = '1'
                            ll.append(newNode)
                        if ll.find('epsilon') is not None :
                            ll.find('epsilon').text = str(self.epsilon)
                        else:
                            newNode = ET.Element('epsilon')
                            newNode.text = str(self.epsilon)
                            ll.append(newNode)
                        if ll.find('alpha') is not None :
                            ll.find('alpha').text = str(self.alpha)
                        else:
                            newNode = ET.Element('alpha')
                            newNode.text = str(self.alpha)
                            ll.append(newNode)
                        if ll.find('gamma') is not None :
                            ll.find('gamma').text = str(self.gamma)
                        else:
                            newNode = ET.Element('gamma')
                            newNode.text = str(self.gamma)
                            ll.append(newNode)
        tree.write(rl + self.simulationFile)


    def copyToResultsLocation(self, x):
        rl = self.resultsLocation
        if not os.path.exists(rl):
            os.makedirs(rl)
        outfileStr = "NoMASS-" + str(x).zfill(5) + ".out"
        copyfile(self.runLoc(x)+self.outFile, rl+outfileStr)
        self.createPandasFiles(x, self.runLoc(x)+self.outFile)
        outfileStr = "SimulationConfig-" + str(x).zfill(5) + ".xml"
        copyfile(self.runLoc(x)+self.simulationFile, rl+outfileStr)
        if self.learning():
            for f in glob.glob(self.runLoc(x) + self.appLearningFile):
                path = os.path.dirname(f)
                filename = os.path.basename(f)
                copyfile(f, rl + filename + "." + str(x).zfill(5))
                ll = self.learntDataLocation()
                copyfile(f, ll + filename)

    def runLoc(self, x):
        return self.runLocation + self.simulationLocation + str(x) +  "/"

    def learntDataLocation(self):
        if self.learntData == "":
            self.learntData = self.resultsLocation + "learningdata/"
            if not os.path.exists(self.learntData):
                os.makedirs(self.learntData)
        return self.learntData

    def copyToRunLocation(self, x):
        rl = self.runLoc(x)
        if not os.path.exists(rl):
            os.makedirs(rl)
        copyfile(self.locationOfNoMASS+self.NoMASSstr, rl+self.NoMASSstr)
        con = self.configurationDirectory
        copyfile(con + self.simulationFile, rl + self.simulationFile)
        copyfile(con + self.activityFile, rl + self.activityFile)
        copyfile(con + self.largeApplianceFile, rl + self.largeApplianceFile)
        copyfile(con + self.PVFile, rl + self.PVFile)
        if os.path.isdir(rl + self.smallApplianceFolder):
            rmtree(rl + self.smallApplianceFolder)
        copytree(con + self.smallApplianceFolder, rl + self.smallApplianceFolder)
        ll = self.learntDataLocation()
        if self.learning():
            if not os.path.exists(ll):
                os.makedirs(ll)
        for f in glob.glob(ll + self.appLearningFile):
            path = os.path.dirname(f)
            filename = os.path.basename(f)
            copyfile(f, rl + filename)



    def makeExecutable(self, x):
        rl = self.runLoc(x)
        nomassexe = rl+self.NoMASSstr
        st = os.stat(nomassexe)
        os.chmod(nomassexe, st.st_mode | stat.S_IEXEC)

    def run(self, x):
        rl = self.runLoc(x)
        p = subprocess.Popen('./' + self.NoMASSstr, cwd=rl)
        p.communicate()

    def createPandasFiles(self, x, filename):
        if self.PandasFiles:
            a = pd.read_csv(filename)
            a['nsim'] = x
            self.pddf = self.pddf.append(a, ignore_index=True)
