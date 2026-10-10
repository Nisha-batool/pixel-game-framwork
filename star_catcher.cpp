#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

class StarCatcher : public olc::PixelGameEngine
{
public:
	StarCatcher()
	{
		sAppName = "Star Catcher";
	}

	bool OnUserCreate() override
	{
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		ResetGame();
		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		if (m_gameOver)
		{
			DrawGameOver();
			if (GetKey(olc::Key::ENTER).bPressed)
				ResetGame();
			return true;
		}

		if (GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld)
			m_playerX -= m_playerSpeed * elapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld)
			m_playerX += m_playerSpeed * elapsedTime;

		m_playerX = std::clamp(m_playerX, 16.0f, static_cast<float>(ScreenWidth() - 16));

		m_spawnTimer += elapsedTime;
		const float spawnInterval = std::max(0.35f, 0.85f - m_score * 0.01f);
		if (m_spawnTimer >= spawnInterval)
		{
			SpawnFallingObject();
			m_spawnTimer = 0.0f;
		}

		UpdateFallingObjects(elapsedTime);
		DrawGame();
		return true;
	}

private:
	struct FallingObject
	{
		float x;
		float y;
		float speed;
		bool isBomb;
	};

	void ResetGame()
	{
		m_playerX = ScreenWidth() / 2.0f;
		m_score = 0;
		m_lives = 3;
		m_spawnTimer = 0.0f;
		m_gameOver = false;
		m_objects.clear();
	}

	void SpawnFallingObject()
	{
		const float x = static_cast<float>(8 + std::rand() % (ScreenWidth() - 16));
		const float speed = 55.0f + static_cast<float>(std::rand() % 36) + m_score * 1.5f;
		const bool isBomb = std::rand() % 4 == 0;
		m_objects.push_back({ x, -6.0f, speed, isBomb });
	}

	void UpdateFallingObjects(float elapsedTime)
	{
		const float playerY = static_cast<float>(ScreenHeight() - 24);

		for (FallingObject& object : m_objects)
		{
			object.y += object.speed * elapsedTime;

			const bool overlapsPlayer =
				object.x + 5.0f >= m_playerX - 14.0f &&
				object.x - 5.0f <= m_playerX + 14.0f &&
				object.y + 5.0f >= playerY &&
				object.y - 5.0f <= playerY + 8.0f;

			if (overlapsPlayer)
			{
				object.y = static_cast<float>(ScreenHeight() + 10);
				if (object.isBomb)
					--m_lives;
				else
					++m_score;
			}
		}

		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(),
				[this](const FallingObject& object)
				{
					return object.y > ScreenHeight() + 10;
				}),
			m_objects.end());

		if (m_lives <= 0)
			m_gameOver = true;
	}

	void DrawGame()
	{
		Clear(olc::Pixel(12, 18, 38));
		DrawString(8, 8, "STAR CATCHER", olc::WHITE, 1);
		DrawString(8, 20, "SCORE: " + std::to_string(m_score), olc::YELLOW, 1);
		DrawString(ScreenWidth() - 58, 20, "LIVES: " + std::to_string(m_lives), olc::WHITE, 1);
		DrawString(8, ScreenHeight() - 10, "MOVE: A/D OR ARROWS", olc::Pixel(150, 170, 200), 1);

		for (const FallingObject& object : m_objects)
		{
			const int x = static_cast<int>(object.x);
			const int y = static_cast<int>(object.y);
			if (object.isBomb)
			{
				FillCircle(x, y, 5, olc::RED);
				DrawCircle(x, y, 5, olc::Pixel(255, 180, 120));
			}
			else
			{
				FillCircle(x, y, 4, olc::YELLOW);
				DrawLine(x - 6, y, x + 6, y, olc::Pixel(255, 240, 160));
				DrawLine(x, y - 6, x, y + 6, olc::Pixel(255, 240, 160));
			}
		}

		const int playerX = static_cast<int>(m_playerX);
		const int playerY = ScreenHeight() - 24;
		FillRect(playerX - 14, playerY, 28, 8, olc::Pixel(70, 190, 255));
		FillRect(playerX - 10, playerY - 3, 20, 3, olc::Pixel(160, 225, 255));
	}

	void DrawGameOver()
	{
		Clear(olc::Pixel(12, 18, 38));
		DrawString(ScreenWidth() / 2 - 30, ScreenHeight() / 2 - 16, "GAME OVER", olc::RED, 1);
		DrawString(ScreenWidth() / 2 - 34, ScreenHeight() / 2, "SCORE: " + std::to_string(m_score), olc::YELLOW, 1);
		DrawString(ScreenWidth() / 2 - 55, ScreenHeight() / 2 + 16, "PRESS ENTER TO PLAY", olc::WHITE, 1);
	}

	float m_playerX = 0.0f;
	float m_playerSpeed = 120.0f;
	float m_spawnTimer = 0.0f;
	int m_score = 0;
	int m_lives = 3;
	bool m_gameOver = false;
	std::vector<FallingObject> m_objects;
};

int main()
{
	StarCatcher game;
	if (game.Construct(256, 240, 4, 4))
		game.Start();
	return 0;
}
