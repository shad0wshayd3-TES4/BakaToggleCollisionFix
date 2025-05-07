namespace Hooks
{
	namespace hkToggleCollision
	{
		static bool bCollisionEnabled{ true };

		static bool ToggleCollision(const RE::SCRIPT_PARAMETER*, const char*, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
		{
			UE::UCheatManager* CheatManager{ nullptr };
			if (auto Engine = UE::UEngine::GetSingleton()) {
				if (auto World = Engine->GetCurrentPlayWorld(nullptr)) {
					if (auto PlayerController = UE::UGameplayStatics::GetPlayerController(World, 0)) {
						CheatManager = *RE::stl::adjust_pointer<UE::UCheatManager*>(PlayerController, 0x400);
					}
				}
			}

			if (CheatManager) {
				if (bCollisionEnabled) {
					CheatManager->Ghost();
				}
				else {
					CheatManager->Walk();
				}

				bCollisionEnabled = !bCollisionEnabled;
				if (auto MenuConsole = RE::MenuConsole::Instance(true)) {
					MenuConsole->PrintLine("Collision -> %s", bCollisionEnabled ? "On" : "Off");
				}
			}
			return true;
		}

		static void Install()
		{
			if (auto func = RE::SCRIPT_FUNCTION::LocateConsoleCommand("ToggleCollision"sv)) {
				func->executeFunction = ToggleCollision;
			}
		}
	}

	class hkDisableText
	{
	public:
		static void ClientMessage(void*, void*, UE::FName, float)
		{
			// may do something with this later
			// _ClientMessage(a_this, a_msg, a_type, a_duration);
			return;
		}

		inline static REL::Hook _ClientMessage{ REL::Offset(0x2F8C090), 0x64, ClientMessage };
	};

	class hkLoadGame
	{
	public:
		static void LoadGame(void* a_this)
		{
			hkToggleCollision::bCollisionEnabled = true;
			return _LoadGame(a_this);
		}

		inline static REL::Hook _LoadGame{ REL::Offset(0x6738A10), 0x1D12, LoadGame };
	};

	static void Install()
	{
		hkToggleCollision::Install();
	}
}

namespace
{
	void MessageHandler(OBSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type)
		{
		case OBSE::MessagingInterface::kPostLoad:
			Hooks::Install();
			break;
		default:
			break;
		}
	}
}

OBSE_PLUGIN_LOAD(const OBSE::LoadInterface* a_obse)
{
	OBSE::Init(a_obse, { .trampoline = true });
	OBSE::GetMessagingInterface()->RegisterListener(MessageHandler);
	return true;
}
