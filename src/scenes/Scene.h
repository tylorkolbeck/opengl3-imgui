#pragma once

#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace scene
{
	class Scene
	{
	public:
		Scene() {};
		virtual ~Scene() {};

		virtual void OnUpdate(float deltaTime) {};
		virtual void OnRender() {};
		virtual void OnImGuiRender() {};
	};

	class SceneMenu : public Scene
	{
	public:
		SceneMenu(Scene*& currentScenePointer);

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "Registering scene " << name << std::endl;
			m_Scenes.push_back(std::make_pair(name, []() {return new T(); }));
		}
	private:
		Scene*& m_CurrentScene;
		std::vector<std::pair<std::string, std::function<Scene* ()>>> m_Scenes;
	};
}
