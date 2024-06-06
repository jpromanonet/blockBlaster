#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <random>

class BlockBlaster : public olc::PixelGameEngine
{
private:
    const int nBlockSize = 16;
    const int nGridWidth = 10;
    const int nGridHeight = 20;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<int>> currentBlock;
    std::vector<std::vector<int>> nextBlock;
    std::vector<olc::Pixel> blockColors;
    int nCurrentBlockX, nCurrentBlockY;
    int nScore;
    float fBlockFallTime;
    float fBlockFallSpeed;

public:
    BlockBlaster()
    {
        sAppName = "Block Blaster";
    }

    bool OnUserCreate() override
    {
        grid.resize(nGridHeight);
        for (int i = 0; i < nGridHeight; i++)
            grid[i].resize(nGridWidth, 0);

        currentBlock = GetRandomBlock();
        nextBlock = GetRandomBlock();
        nCurrentBlockX = nGridWidth / 2 - static_cast<int>(currentBlock[0].size()) / 2;
        nCurrentBlockY = 0;
        nScore = 0;
        fBlockFallTime = 0.0f;
        fBlockFallSpeed = 0.5f; // Blocks fall every 0.5 seconds

        blockColors = { olc::RED, olc::GREEN, olc::BLUE, olc::YELLOW, olc::MAGENTA, olc::CYAN, olc::WHITE };

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);

        if (IsGameOver())
        {
            DrawString(4, 4, "Game Over! Press R to restart", olc::WHITE, 2);
            if (GetKey(olc::Key::R).bPressed)
                OnUserCreate();
            return true;
        }

        fBlockFallTime += fElapsedTime;

        if (GetKey(olc::Key::LEFT).bPressed && IsValidMove(currentBlock, nCurrentBlockX - 1, nCurrentBlockY))
            nCurrentBlockX--;
        if (GetKey(olc::Key::RIGHT).bPressed && IsValidMove(currentBlock, nCurrentBlockX + 1, nCurrentBlockY))
            nCurrentBlockX++;
        if (GetKey(olc::Key::DOWN).bPressed && IsValidMove(currentBlock, nCurrentBlockX, nCurrentBlockY + 1))
            nCurrentBlockY++;

        if (GetKey(olc::Key::UP).bPressed)
        {
            auto rotatedBlock = RotateBlock(currentBlock);
            if (IsValidMove(rotatedBlock, nCurrentBlockX, nCurrentBlockY))
                currentBlock = rotatedBlock;
        }

        if (GetKey(olc::Key::SPACE).bPressed)
        {
            while (IsValidMove(currentBlock, nCurrentBlockX, nCurrentBlockY + 1))
                nCurrentBlockY++;
        }

        if (fBlockFallTime >= fBlockFallSpeed)
        {
            fBlockFallTime = 0;

            if (IsValidMove(currentBlock, nCurrentBlockX, nCurrentBlockY + 1))
                nCurrentBlockY++;
            else
            {
                MergeCurrentBlock();
                ClearLines();
                currentBlock = nextBlock;
                nextBlock = GetRandomBlock();
                nCurrentBlockX = nGridWidth / 2 - static_cast<int>(currentBlock[0].size()) / 2;
                nCurrentBlockY = 0;
            }
        }

        DrawGrid();
        DrawBlock(currentBlock, nCurrentBlockX, nCurrentBlockY, blockColors[0]);
        DrawBlock(nextBlock, nGridWidth + 2, 2, blockColors[1]);
        DrawString(4, nGridHeight * nBlockSize + 4, "Score: " + std::to_string(nScore), olc::WHITE, 2);

        return true;
    }

private:
    std::vector<std::vector<int>> GetRandomBlock()
    {
        // Define different block shapes here
        std::vector<std::vector<int>> block1 = { {1, 1}, {1, 1} };
        std::vector<std::vector<int>> block2 = { {1, 1, 1, 1} };
        std::vector<std::vector<int>> block3 = { {0, 1, 0}, {1, 1, 1} };
        std::vector<std::vector<int>> block4 = { {0, 1, 1}, {1, 1, 0} };
        std::vector<std::vector<int>> block5 = { {1, 1, 0}, {0, 1, 1} };
        std::vector<std::vector<int>> block6 = { {1, 0, 0}, {1, 1, 1} };
        std::vector<std::vector<int>> block7 = { {0, 0, 1}, {1, 1, 1} };

        std::vector<std::vector<int>> blocks[] = { block1, block2, block3, block4, block5, block6, block7 };
        int nIndex = rand() % 7;
        return blocks[nIndex];
    }

    bool IsValidMove(const std::vector<std::vector<int>>& block, int nPosX, int nPosY)
    {
        for (int y = 0; y < static_cast<int>(block.size()); y++)
        {
            for (int x = 0; x < static_cast<int>(block[y].size()); x++)
            {
                if (block[y][x] && (nPosX + x < 0 || nPosX + x >= nGridWidth || nPosY + y >= nGridHeight || grid[nPosY + y][nPosX + x]))
                    return false;
            }
        }
        return true;
    }

    std::vector<std::vector<int>> RotateBlock(const std::vector<std::vector<int>>& block)
    {
        int n = block.size();
        std::vector<std::vector<int>> rotated(n, std::vector<int>(n, 0));
        for (int y = 0; y < n; y++)
            for (int x = 0; x < n; x++)
                rotated[x][n - y - 1] = block[y][x];
        return rotated;
    }

    void MergeCurrentBlock()
    {
        for (int y = 0; y < static_cast<int>(currentBlock.size()); y++)
        {
            for (int x = 0; x < static_cast<int>(currentBlock[y].size()); x++)
            {
                if (currentBlock[y][x])
                    grid[nCurrentBlockY + y][nCurrentBlockX + x] = 1;
            }
        }
    }

    void ClearLines()
    {
        for (int y = 0; y < nGridHeight; y++)
        {
            if (std::count(grid[y].begin(), grid[y].end(), 1) == nGridWidth)
            {
                for (int yy = y; yy > 0; yy--)
                    grid[yy] = grid[yy - 1];
                grid[0].assign(nGridWidth, 0);
                nScore += 100;
            }
        }
    }

    bool IsGameOver()
    {
        for (int x = 0; x < nGridWidth; x++)
        {
            if (grid[0][x])
                return true;
        }
        return false;
    }

    void DrawGrid()
    {
        for (int y = 0; y < nGridHeight; y++)
        {
            for (int x = 0; x < nGridWidth; x++)
            {
                DrawRect(x * nBlockSize, y * nBlockSize, nBlockSize, nBlockSize, olc::WHITE);
                if (grid[y][x])
                    FillRect(x * nBlockSize, y * nBlockSize, nBlockSize, nBlockSize, olc::WHITE);
            }
        }
    }

    void DrawBlock(const std::vector<std::vector<int>>& block, int nPosX, int nPosY, olc::Pixel color)
    {
        for (int y = 0; y < static_cast<int>(block.size()); y++)
        {
            for (int x = 0; x < static_cast<int>(block[y].size()); x++)
            {
                if (block[y][x])
                    FillRect((nPosX + x) * nBlockSize, (nPosY + y) * nBlockSize, nBlockSize, nBlockSize, color);
            }
        }
    }
};

int main()
{
    BlockBlaster game;
    if (game.Construct(320, 480, 2, 2))
        game.Start();

    return 0;
}

