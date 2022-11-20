# Instruction
(1) After downloading the code, add the NetworkingClient project and the NetworkingServer project to your solution(My recommendation will be under the Engine directory). Set their application type to be .lib and .exe separately. Then add the ws2_32.lib to the NetworkingServer project (Configuration properties - Linker - Input - Additional Dependencies). Finally, add the EngineDefault.props to both of them so that the build results will be located in the same place as other Engine projects’ build results.

(2) The MyGame project(or any other project you put your gameplay code in) needs to reference the NetworkingClient project. Then add the ws2_32.lib also to the MyGame project.

(3) Build the NetworkingServer project. Then the NetworkingServer.exe will appear at where you want. You need to run it before you run your game to get things work.

(4) Last step, to use the Sync function from MyNetworkClient, you need to call StartClient() and EndClient() at the beginning and the ending of your game. Then you can call Sync() anywhere you want in between the above 2 functions.