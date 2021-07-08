#include "gpc_vtable_parser.h"

namespace gpc {
	void VTableParser::ParseVTable(uint64_t* objAddr) {
		m_objAddr = objAddr;
		m_vPtr = m_objAddr;
		m_vTableAddr = (uint64_t*)*(uint64_t*)m_vPtr;
		//get the virtual table pointer, the first 4 bytes of the object
		uint64_t* vptr = (uint64_t*)m_objAddr;
		uint64_t vTableAddr = *vptr;

		//for (int i = 0; i < 3; ++i) {
		//	uint64_t vFuncAddr = *((uint64_t*)vTableAddr + i);
		//	m_vFunAddrs.push_back(vFuncAddr);
		//}
	}

	void VTableParser::ParseDXInterfaceVTable(fs::path headerFile, std::map<std::string, std::map<std::string, GPCAPIHookInfo>>& vTableMap) {
		//d3d12 inteface inheritance diagram: https://drive.google.com/file/d/14uAyC8fHfIg-eSnnJMuV9oYypBZjnqpg/view
		//regex visualization website: https://regexper.com/
		vTableMap["IUnknown"] = {
			{"AddRef", GPCAPIHookInfo("IUnknown", "AddRef", 0, GPC_HOOK_POS_ALL)},
			{"Release", GPCAPIHookInfo("IUnknown","Release", 1, GPC_HOOK_POS_ALL)}
		};

		//vTableMap["ID3D12Object"] = {
		//	{"GetPrivateData", GPCAPIHookInfo("ID3D12Object","GetPrivateData", 2, GPC_HOOK_POS_NONE)},
		//	{"SetPrivateData", GPCAPIHookInfo("ID3D12Object","SetPrivateData", 3, GPC_HOOK_POS_NONE)},
		//	{"SetPrivateDataInterface", GPCAPIHookInfo("ID3D12Object","SetPrivateDataInterface", 4, GPC_HOOK_POS_NONE)},
		//	{"SetName", GPCAPIHookInfo("ID3D12Object","SetName", 5, GPC_HOOK_POS_NONE)}
		//};

		std::ifstream ifs;
		ifs.open(headerFile.string().c_str());
		if (ifs.good()) {
			std::string line;
			bool isInterfacePreviousLine = false;
			bool isInterfaceStart = false;
			int interfaceNum = 0;
			/*
			interface:     ID3D12Device4 : public ID3D12Device3
			method: virtual HRESULT STDMETHODCALLTYPE CreateCommandList1(
			*/
			std::string interfaceName = "";
			int interfaceOffset = 0;
			std::map<std::string, GPCAPIHookInfo> interfaceVtable = {};
			while (std::getline(ifs, line)) {
				if (isInterfacePreviousLine) {
					if (interfaceVtable.size() > 0) {
						vTableMap[interfaceName] = interfaceVtable;
						interfaceName = "";
						interfaceVtable.clear();
					}

					//parse interface name
					std::regex pattern("\\s*(.*?)\\s* : public (.*)\\s*");
					std::smatch match;
					std::regex_search(line, match, pattern);
					std::string baseInterface = match[2].str();
					std::string derivedInterface = match[1].str();

					interfaceName = derivedInterface;
					if (vTableMap.find(baseInterface) != vTableMap.end()) {
						interfaceVtable = vTableMap[baseInterface];
						interfaceOffset = vTableMap[baseInterface].size(); //init
					}
					else {
						interfaceOffset = 0;
					}
					std::cout << "\ninterface name: " << interfaceName << " derived from interface: " << baseInterface << std::endl;
					isInterfaceStart = true;
					isInterfacePreviousLine = false;
				}
				if (line.find("MIDL_INTERFACE") != std::string::npos) {
					isInterfacePreviousLine = true;
					interfaceNum++;
				}
				if (line.find("/* C style interface */") != std::string::npos) {
					isInterfaceStart = false;
				}

				if (isInterfaceStart) {
					//parse method name
					std::regex pattern(".*virtual HRESULT STDMETHODCALLTYPE\\s*(.*?)\\(.*");
					std::smatch match;
					std::regex_search(line, match, pattern);
					std::string methodName = match[1].str();

					if (match.size() > 0) {
						interfaceOffset++;
						interfaceVtable[methodName] = GPCAPIHookInfo(interfaceName.c_str(), methodName.c_str(), interfaceOffset, GPC_HOOK_POS_ALL);
						std::cout << "method name: " << match[1] << "virtual offset: " << interfaceOffset << std::endl;
					}
				}
			}
			std::cout << "Total interface number is: " << interfaceNum << std::endl;
		}
	}

	void VTableManager::GetDXHeaders(fs::path sdkPath) {
		/*
		* shared: communications between user mode & kernel mode
		* um: user mode
		* km: kernel mode
		*/
		std::cout << "Start to get dx header files" << std::endl;
		std::set<std::string> headerList = {
			"d3d12.h", //um
			"dxgi.h", //shared
			"dxgi1_2.h",
			"dxgi1_3.h",
			"dxgi1_4.h",
			"dxgi1_5.h",
			"dxgi1_6.h"
		};

		for (auto& file : fs::recursive_directory_iterator(sdkPath)) {
			std::string filename = file.path().filename().string();
			if (headerList.find(filename) != headerList.end())
				m_dxHeaders.push_back(file.path().string());
		}
		std::cout << "Get dx header done" << std::endl;
	}

	void VTableManager::GetDXVTables() {
		std::cout << "Start to get dx vtables" << std::endl;
		GetDXHeaders();

		if (!m_dxHeaders.empty()) {
			for (auto& header : m_dxHeaders) {
				std::cout << "\nStart to parse: " << header.string() << std::endl;
				m_vTableParser->ParseDXInterfaceVTable(header, m_dxIVtable);
				std::cout << "parse done" << std::endl;
			}
		}

		fs::path vtableFile = fs::current_path() / "vtable.csv";
		DumpVTable(vtableFile);
	}

	void VTableManager::DumpVTable(fs::path outFile) {
		std::cout << "Start to dump vtable file" << std::endl;
		const char* interfaceWhiteList[10] = {
			"ID3D12Device",
			"ID3D12Device1",
			"ID3D12Device2",
			"ID3D12Device3",
			"ID3D12Device4",
			"ID3D12Device5",
			"IDXGISwapChain"
		};

		std::ofstream ofs;
		ofs.open(outFile.string().c_str());
		if (ofs.good()) {
			ofs << "interface, method, offset, hook\n";
			for (auto interfaceEle : m_dxIVtable) {
				for (auto methodEle : interfaceEle.second) {
					ofs << interfaceEle.first << "," \
						<< methodEle.first << "," \
						<< methodEle.second.vTableOffset << "," \
						<< int(methodEle.second.hookPos) << "\n";
				}
			}
		}
	}
}