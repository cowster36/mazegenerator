#include <windows.h>
#include <stdint.h>
#include "resource.h"

char windowClassName[] = "C Window";

int screenWidth, screenHeight;
int tileSize = 20;
int numCols, numRows;

uint32_t * screenBuffer;
BITMAPINFO bitmap_info;

char * mazeData;

void generateMaze() {
    for (int i = 0; i < numCols * numRows * 2; i++) {
        mazeData[i] = 1;
    }

    int currentIndex = 0;
    char * hasVisitedCell = malloc(numCols * numRows);
    for (int i = 0; i < numCols * numRows; i++) {
        hasVisitedCell[i] = 0;
    }

    int * visitedStack = malloc(numCols * numRows * sizeof(int));
    visitedStack[0] = currentIndex;
    int visitedStackLength = 1;

    while(1 == 1) {
        char numNeighbors = 0;
        char neighbors [4] = { 0, 0, 0, 0 };

        if (currentIndex % numCols != 0 && hasVisitedCell[currentIndex - 1] == 0) {
            neighbors[numNeighbors] = 0;
            numNeighbors += 1;
        } 
        
        if (currentIndex % numCols != numCols - 1 && hasVisitedCell[currentIndex + 1] == 0) {
            neighbors[numNeighbors] = 1;
            numNeighbors += 1;
        } 
        
        if (currentIndex + numCols < numCols * numRows && hasVisitedCell[currentIndex + numCols] == 0) {
            neighbors[numNeighbors] = 2;
            numNeighbors += 1;
        }
        
        if (currentIndex - numCols > 0 && hasVisitedCell[currentIndex - numCols] == 0) { 
            neighbors[numNeighbors] = 3;
            numNeighbors += 1;
        }

        if (numNeighbors > 0) {
            int index = rand() % numNeighbors;
            if (neighbors[index] == 0) {
                currentIndex -= 1;
                mazeData[currentIndex * 2] = 0;
            } else if (neighbors[index] == 1) {
                mazeData[currentIndex * 2] = 0;
                currentIndex += 1;
            } else if (neighbors[index] == 2) {
                mazeData[currentIndex * 2 + 1] = 0;
                currentIndex += numCols;
            } else if (neighbors[index] == 3) {
                currentIndex -= numCols;
                mazeData[currentIndex * 2 + 1] = 0;
            }

            hasVisitedCell[currentIndex] = 1;
            visitedStackLength += 1;
            visitedStack[visitedStackLength - 1] = currentIndex;
        } else if (visitedStackLength > 1) {
            visitedStackLength -= 1;
            currentIndex = visitedStack[visitedStackLength - 1];
        } else {
            break;
        }
    }

    free(hasVisitedCell);
    free(visitedStack);
};

void drawTile(int index) {
    if (mazeData[index] == 1) {
        int x = (index / 2) % numCols * tileSize + tileSize;
        int startingY = (int)( (index / 2) / numCols ) * tileSize; 

        for (int y = startingY; y < startingY + tileSize; y++) {
            if (x + y * screenWidth < screenWidth * screenHeight)
            screenBuffer[x + y * screenWidth] = 0x00FFFFFF;
        }
    }

    if (mazeData[index + 1] == 1) {
        int startingX = (index / 2) % numCols * tileSize;
        int y = (int)((index / 2) / numCols) * tileSize + tileSize;

        for (int x = startingX; x < startingX + tileSize; x++) {
            if(x + y * screenWidth < screenWidth * screenHeight)
            screenBuffer[x + y * screenWidth] = 0x00FFFFFF;
        }
    }
};

void drawBackground() {
    for (int i = 0; i < screenWidth * screenHeight; i++) {
        screenBuffer[i] = 0x00000000;
    }

    for (int y = 0; y < screenHeight; y++) {
        screenBuffer[y * screenWidth] = 0x00FFFFFF;
        screenBuffer[screenWidth - 1 + y * screenWidth] = 0x00FFFFFF;
    }

    for (int x = 0; x < screenWidth; x++) {
        screenBuffer[x] = 0x00FFFFFF;
        screenBuffer[x + (screenHeight - 1) * screenWidth] = 0x00FFFFFF;
    }
};

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        switch (wParam) {
        case ID_OPTIONS_GEN:
            drawBackground();

            generateMaze();

            for (int i = 0; i < numCols * numRows * 2; i += 2) {
                drawTile(i);
            }
            break;
        case ID_OPTIONS_EXIT:
            PostMessage(windowHandle, WM_CLOSE, 0, 0);
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(windowHandle);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(windowHandle, message, wParam, lParam);
        break;
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    MSG message;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = windowClassName;
    wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    HWND windowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, windowClassName,
        "Maze Generator in C", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480,
        NULL, NULL,
        hInstance, NULL);

    ShowWindow(windowHandle, nCmdShow);
    UpdateWindow(windowHandle);

    RECT rect;
    GetClientRect(windowHandle, &rect);
    screenWidth = rect.right - rect.left;
    screenHeight = rect.bottom - rect.top;

    screenBuffer = malloc(screenWidth * screenHeight * 4);

    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = screenWidth;
    bitmap_info.bmiHeader.biHeight = -screenHeight;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    HDC DCHandle = GetDC(windowHandle);

    numCols = (int) (screenWidth / tileSize);
    numRows = (int) (screenHeight / tileSize) + 1; 
    mazeData = malloc(numCols * numRows * 2);

    drawBackground();

    generateMaze();
    
    for (int i = 0; i < numCols * numRows * 2; i += 2) {
        drawTile(i);
    }

    while (IsWindow(windowHandle)) {
        while (PeekMessage(&message, windowHandle, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        StretchDIBits(DCHandle,
            0, 0,
            screenWidth, screenHeight,
            0, 0,
            screenWidth, screenHeight,
            screenBuffer, &bitmap_info,
            DIB_RGB_COLORS, SRCCOPY);
    }

    free(mazeData);
    free(screenBuffer);

    return message.wParam;
}
