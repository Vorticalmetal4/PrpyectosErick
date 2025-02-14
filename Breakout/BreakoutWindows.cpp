#include "BreakoutWindows.h"
#include "CommonFiles/Renderer.h"
#include "CommonFiles/CollisionSystem.h"
#include "Player.h"
#include "Ball.h"
#include "Brick.h"
#include "HUD.h"
#include "Power.h"
#include "Ray.h"
#include "Inih/cpp/INIReader.h"

#include <vector>
#include <ctime>

int main(int argc, int** argv) 
{

    INIReader ConFile("InitialData.ini"); 

    if (ConFile.ParseError() < 0)
        ConFile.PrintError("BreakoutWindow could not find ConFile"); 
    
    Renderer Rend;
    CollisionSystem CollisionDetector;
    int PowerProbability = ConFile.GetInteger("Power", "Probability", 0);
    const int BricksColumns = ConFile.GetInteger("Brick", "BricksColumns", 0); 
    const int BricksRows = ConFile.GetInteger("Brick", "BricksRows", 0);

    int i, j, k;
    GameData Data;
    Data.BricksRemaining = BricksColumns * BricksRows;
    Data.Lives = ConFile.GetInteger("HUD", "Lives", 0);

    bool success = Rend.Initialize(ConFile.GetString("Window", "Name", "Error"),
                                   ConFile.GetInteger("Window", "TopLeftXCoordinate", 100),
                                   ConFile.GetInteger("Window", "TopLeftYCoordinate", 20),
                                   ConFile.GetInteger("Window", "Width", 1080),
                                   ConFile.GetInteger("Window", "Height", 700),
                                   ConFile.GetInteger("Window", "Flags", 0),
                                   ConFile.GetString("Window", "Font", "Error"));

    const float BricksSeparation = (Rend.getWindowWidth() - BricksColumns * (float)ConFile.GetInteger("Brick", "width", 97)) / BricksColumns;
    Ray PlayersRay = Ray(&CollisionDetector);
    Player Player1 = Player(&Rend, &PlayersRay); 
    HUD PHUD = HUD(&Rend, &Player1); 
    Ball MainBall = Ball(&Rend, &CollisionDetector,&Player1); 

    vector<vector<Brick>> Bricks;

    for (i = 0; i < BricksColumns; i++)
    {
        vector<Brick> BricksTem;
        for(j = 0; j < BricksRows; j++)
        {
            Brick BrickT = Brick(&Rend, &CollisionDetector,i, j, BricksSeparation);
            BricksTem.push_back(BrickT);
        }

        Bricks.push_back(BricksTem);
    }


    vector<Power> Powers; 
    for(i = 0; i < 3; i++)
    {
        Power NPower = Power(&Player1, &Rend);
        Powers.push_back(NPower);
    }

    srand((unsigned int)time(NULL));

    if (success)
    {
        while (Rend.getIsRunning())
        {
            Rend.ProcessInput();
            Rend.UpdateGame();
            Rend.ClearRender();
            for (i = 0; i < BricksColumns; i++)
            { 
                for (j = 0; j < BricksRows; j++)
                {
                    if (Bricks[i][j].getActive())
                    {
                        if (Bricks[i][j].CheckCollition(&MainBall)) 
                        {
                            if(Bricks[i][j].getTraitor() == false)
                                Data.BricksRemaining--;
                            if ((rand() % (100) + 1) <= PowerProbability)
                            {
                                for (k = 0; k < Powers.size(); k++) 
                                {
                                    if (Powers[k].getActive() == false)
                                    {
                                        Powers[k].SetData(Bricks[i][j].getPosition()->x, Bricks[i][j].getPosition()->y + 10, true);
                                        break;
                                    }
                                }
                                
                            }
                        }
                        Bricks[i][j].Draw(j);
   
                        if (Player1.CheckLasersCollition(&Bricks[i][j])) 
                        {
                            if (Player1.getPower() == 'T') 
                            {    //Check if alredy exists a traitorous brick in the position of the new traitor
                                Bricks[i][j].setTraitor(true);
                                for (k = 0; k < BricksRows; k++) 
                                {
                                    if (k != j)
                                        if (Bricks[i][j].getPosition()->x == Bricks[i][k].getPosition()->x && Bricks[i][j].getPosition()->y == Bricks[i][k].getPosition()->y)
                                            Bricks[i][j].setActive(false);
                                }
                            }

                            Player1.NoPower();
                            Data.BricksRemaining--;
                        }
                        
                    }

                }
            }

            if (Data.BricksRemaining > 0 && Data.Lives > 0)
            {
                if (MainBall.Update(PHUD.getPause()) == false)
                    Data.Lives--;

                Player1.Update(PHUD.getPause());

                for (i = 0; i < Powers.size(); i++)
                {
                    if (Powers[i].getActive())
                    {
                        if (Powers[i].CheckCollision(PHUD.getPause()))
                            Powers[i].SetData(-50, -50, false);
                        else
                            Powers[i].Update();
                    }
                }
                
            }

            PHUD.UpdateHUD(&Data); 
            Rend.GenerateOutput();
        }
    }
        
    Rend.FreeMemory();
    Rend.Shutdown();

    return 0;
}

