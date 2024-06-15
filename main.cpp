#ifndef UNICODE
#define UNICODE
#endif 


//------------------
//Explicit libraries

//API
#include <windows.h>
#include <commdlg.h>
#include <WindowsX.h>

//Frequently used
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//Task specific
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <limits>
//End of Explicit libraries
//------------------------

int debug = 1;


//Project settings
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
#define DRAW_HEIGHT 800
#define DRAW_WIDTH 800


//---------------
//My headers
#include "colors.h" //independent
#include "structs.h" //geometry core, include after vector and math
#include "screen_buffer.h" //include after windows and colors and structs
#include "zbuffer.h" // after sizes define, before camera
#include "camera.h" //include after structs
#include "reader.h" //after structs and camera, because contains settings


//End of my headers
//---------------



//Main
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
){
	
	//Structure with info about app
	WNDCLASSEX wcex;
	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WindowProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = L"Nick";
	wcex.lpszClassName  = L"Sample Window Class";
	wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	
	//Register window
	if (!RegisterClassEx(&wcex))
	{
	   MessageBox(NULL,
	      L"Call to RegisterClassEx failed!",
	      L"Windows Desktop Guided Tour",
	      MB_OK); //I don't know what parameter uType is for, works with NULL
	
	   return 1;
	}
	
	static TCHAR szWindowClass[] = L"DesktopApp";
	static TCHAR szTitle[] = L"Project";

	const wchar_t CLASS_NAME[]  = L"Sample Window Class";
	HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Nikita Babich graphics project",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Position and size
        //CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        20,20,WINDOW_WIDTH,WINDOW_HEIGHT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
        
        
	if (!hwnd)
	{
	   MessageBox(NULL,
	      L"Call to CreateWindowEx failed!",
	      L"Windows Desktop Guided Tour",
	      MB_OK);
	
	   return 1;
	}
	
	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	
	// Segment for setup
	
	//first read here
	
	read_config();
	readVtkFile(filepath1, allpoints, scene);
	
	InitializeBitmapInfo(&bmi);
	InitializeBuffer();
	
	ShowWindow(hwnd, nCmdShow);
	//srand(time(NULL));
	//main_contour = rcont(6);
	
	//UpdateWindow(hwnd);
	printf("\nWindow is running");
	
	
	
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
	}

	return (int) msg.wParam;
};


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int mouseX,mouseY;
	//HDC scrDC;
	//PAINTSTRUCT ps;
    //HDC hdc;
    
    switch (uMsg)
    {
    case WM_DESTROY:
    	printf("\nWindow is closing");
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
			drawScene();
			UPDATE;
        }
        return 0;
        
    case WM_LBUTTONDOWN:
            mouseX = LOWORD(lParam);
            mouseY = HIWORD(lParam);
            printf("\n Left mouse click \t %d %d ",mouseX,mouseY);
            
			//InvalidateRect(hwnd, NULL, FALSE);
            break;
            
    case WM_MOUSEMOVE: 
    		{	
    		
    		}
    		break;
            
    case WM_RBUTTONDOWN:
    		//main_contour.clear();
			//InvalidateRect(hwnd, NULL, FALSE);
            break;
            
    case WM_KEYDOWN:
        // Handle keydown event
        switch (wParam) {
            case VK_LEFT:  rot(LEFT); break;
            case VK_RIGHT: rot(RIGHT);  break;
            case VK_UP: rot(UP); break; 
			case VK_DOWN: rot(DOWN);  break; 
			case VK_HOME: 
				// Process the HOME key. 
				break; 
			case VK_END: 
				// Process the END key. 
				break; 
			case VK_INSERT: 
				// Process the INS key. 
				break; 
			case VK_DELETE: 
				// Process the DEL key. 
				break; 
			case VK_ESCAPE: 
				read_config();
				break; 
			case VK_F2: 
				// Process the F2 key. 
				break; 
				
			case 'Q': turn(LEFT);  break;
			case 'E': turn(RIGHT);  break;
			case 'R': turn(UP);  break;
			case 'F': turn(DOWN);  break;
			
			case 'W': move(FORWARD);  break;
			case 'S': move(BACKWARD);  break;
			case 'D': move(RIGHT);  break;
			case 'A': move(LEFT);  break;
			
			
			
			case VK_SPACE: move(UP); break;
			case VK_SHIFT: move(DOWN); break;
			
			case 'I': camera.dist = camera.dist - 0.1;  
				//camera.pos = camera.dist * camera.pos;
				break;
			case 'K': camera.dist = camera.dist + 0.1; 
				//camera.pos = camera.dist * camera.pos;
				 break;
			
			case 'Z': Pmode = !Pmode; printf(" Spherical : %s\n", Pmode ? "true" : "false"); break;
			case 'X': Dmode = !Dmode; printf(" Wireframe : %s\n", Dmode ? "false" : "true"); break;
			case 'C': break;
			case 'V': triangle_method = !triangle_method; printf(" Goraud : %s\n", triangle_method ? "true" : "false"); break;
				
			case '1':
				//read_config();
				readVtkFile(filepath1, allpoints, scene);
				break;
			case '2':
				//read_config();
				readVtkFile(filepath2, allpoints, scene);
				break;
			case '3':
				//read_config();
				readVtkFile(filepath3, allpoints, scene);
				break;
			case '4':
				//read_config();
				readVtkFile(filepath4, allpoints, scene);
				break;
			
			//case 'T': shearMainContour(UP);  break;
			//case 'G': shearMainContour(DOWN);  break;
			//case 'H': shearMainContour(RIGHT);  break;
			//case 'F': shearMainContour(LEFT);  break;
			
			//case 'B': triangle_method = (triangle_method+1)%2;  break;
			//case 'R': main_contour = rcont(10);  break;
			//case 'X': symmetryMainContour();  break;
			//case 'Z': selectedMethod = static_cast<LineMethod>((selectedMethod + 1)%3); printf("\nmethod %d", selectedMethod); break;
			//case 'M': PROGRAM_MODE = static_cast<progState>((PROGRAM_MODE + 1)%6); break;
			default: 
				// Process other non-character keystrokes. 
				break; 
        }
        if(camera.dist<0)camera.dist=1;
        report();
        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case WM_KEYUP:
        // Handle keyup event, might not need those
        switch (wParam) {
            case VK_LEFT:
                break;
            case VK_RIGHT:
                break;
        }
        break;        
        
	default:
      	return DefWindowProc(hwnd, uMsg, wParam, lParam);
      	break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
