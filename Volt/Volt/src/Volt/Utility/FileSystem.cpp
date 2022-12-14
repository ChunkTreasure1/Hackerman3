#include "vtpch.h"
#include "FileSystem.h"

#include "Volt/Core/Application.h"

#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <windows.h>

std::filesystem::path FileSystem::OpenFolder()
{
	HRESULT result;
	IFileOpenDialog* openFolderDialog;

	std::filesystem::path resultPath;

	result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&openFolderDialog));
	if (SUCCEEDED(result))
	{
		IShellItem* startLocation = nullptr;
		result = SHCreateItemFromParsingName(std::filesystem::current_path().c_str(), NULL, IID_IShellItem, (void**)&startLocation);

		if (SUCCEEDED(result))
		{
			openFolderDialog->SetOptions(FOS_PICKFOLDERS);
			openFolderDialog->SetDefaultFolder(startLocation);

			result = openFolderDialog->Show(Volt::Application::Get().GetWindow().GetHWND());
			if (SUCCEEDED(result))
			{
				IShellItem* itemResult;
				result = openFolderDialog->GetResult(&itemResult);

				if (SUCCEEDED(result))
				{
					LPWSTR wstr = NULL;
					result = itemResult->GetDisplayName(SIGDN_FILESYSPATH, &wstr);
					if (SUCCEEDED(result))
					{
						resultPath = wstr;
						CoTaskMemFree(wstr);
					}

					itemResult->Release();
				}
			}
			startLocation->Release();
		}
		openFolderDialog->Release();
	}

	return GetPathRelativeToBaseFolder(resultPath);
}

std::filesystem::path FileSystem::SaveFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	const std::string startDir = std::filesystem::current_path().string();

	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = startDir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}

	return std::string();
}

std::filesystem::path FileSystem::GetDocumentsPath()
{
	TCHAR* path = 0;
	std::filesystem::path documentsPath;
	SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, NULL, &path);
	documentsPath = path;
	CoTaskMemFree(path);

	return documentsPath;
}

bool FileSystem::ShowDirectoryInExplorer(const std::filesystem::path& aPath)
{
	auto absolutePath = std::filesystem::canonical(aPath);
	if (!Exists(absolutePath))
	{
		return false;
	}

	ShellExecute(nullptr, L"explorer", absolutePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	return true;
}

bool FileSystem::OpenFileExternally(const std::filesystem::path& aPath)
{
	{
		auto absolutePath = std::filesystem::canonical(aPath);
		if (!Exists(absolutePath))
		{
			return false;
		}

		ShellExecute(nullptr, L"open", absolutePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		return true;
	}
}

std::filesystem::path FileSystem::OpenFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	const std::string startDir = std::filesystem::current_path().string();

	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = startDir.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}

	return "";
}

bool FileSystem::HasEnvironmentVariable(const std::string& key)
{
	HKEY hKey;
	LPCSTR keyPath = "Environment";
	LSTATUS lOpenStatus = RegOpenKeyExA(HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &hKey);

	if (lOpenStatus == ERROR_SUCCESS)
	{
		lOpenStatus = RegQueryValueExA(hKey, key.c_str(), nullptr, nullptr, nullptr, nullptr);
		RegCloseKey(hKey);
	}

	return lOpenStatus == ERROR_SUCCESS;
}

bool FileSystem::SetEnvVariable(const std::string& key, const std::string& value)
{
	HKEY hKey;
	LPCSTR keyPath = "Environment";
	DWORD createdNewKey;
	LSTATUS lOpenStatus = RegCreateKeyExA(HKEY_CURRENT_USER, keyPath, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, &createdNewKey);
	if (lOpenStatus == ERROR_SUCCESS)
	{
		LSTATUS lSetStatus = RegSetValueExA(hKey, key.c_str(), 0, REG_SZ, (LPBYTE)value.c_str(), (DWORD)value.length() + 1);
		RegCloseKey(hKey);

		if (lSetStatus == ERROR_SUCCESS)
		{
			SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK, 100, nullptr);
			return true;
		}
	}

	return false;
}

std::string FileSystem::GetEnvVariable(const std::string& key)
{
	HKEY hKey;
	LPCSTR keyPath = "Environment";
	DWORD createdNewKey;
	LSTATUS lOpenStatus = RegCreateKeyExA(HKEY_CURRENT_USER, keyPath, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, &createdNewKey);
	if (lOpenStatus == ERROR_SUCCESS)
	{
		DWORD valueType;
		auto* data = new char[512];
		DWORD dataSize = 512;
		LSTATUS status = RegGetValueA(hKey, nullptr, key.c_str(), RRF_RT_ANY, &valueType, (PVOID)data, &dataSize);

		RegCloseKey(hKey);

		if (status == ERROR_SUCCESS)
		{
			std::string result(data);
			delete[] data;
			return result;
		}
	}

	return std::string{};
}