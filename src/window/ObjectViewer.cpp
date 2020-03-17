#include <window/ObjectViewer.hpp>

#include <Application.hpp>
#include <ShitVM.hpp>
#include <svm/GarbageCollector.hpp>
#include <svm/Object.hpp>
#include <svm/Type.hpp>

#include <cstddef>
#include <cstdint>
#include <sstream>

LRESULT ObjectView::Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO: {
		LPMINMAXINFO min = reinterpret_cast<LPMINMAXINFO>(lParam);
		min->ptMinTrackSize.x = 510;
		min->ptMinTrackSize.y = 480;
		return 0;
	}

	case WM_CTLCOLORBTN:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX: {
		const HWND handle = reinterpret_cast<HWND>(lParam);
		COMBOBOXINFO ci = { sizeof(COMBOBOXINFO) };
		GetComboBoxInfo(Children[1].Handle, &ci);

		if (handle == Children[1].Handle) {
			const HDC hdc = reinterpret_cast<HDC>(wParam);
			SetBkColor(hdc, RGB(255, 255, 255));
			return reinterpret_cast<LRESULT>(m_Brush);
		}
		break;
	}
	}
	return Window::Callback(handle, message, wParam, lParam);
}

void ObjectView::Initialize() {
	AddChild(CreateTextBox(this, 80, 12, 200, 25, nullptr))
		.SetWndProc(AddressWndProc);

	AddChild(CreateCheckBox(this, "Managed", 290, 12, 80, 25, nullptr));

	AddChild(CreateButton(this, "Search", 380, 12, 100, 25, CallbackLambda(&) {
		SearchObject();
		return 0;
	}));
}
void ObjectView::Paint(HDC dc) {
	TextOut(dc, 15, 15, "Address:", 8);
	TextOut(dc, 15, 40, m_ObjectType.c_str(), static_cast<int>(m_ObjectType.size()));

	RECT rect;
	SetRect(&rect, 15, 60, 480, 405);
	DrawText(dc, m_ObjectValue.c_str(), static_cast<int>(m_ObjectValue.size()), &rect, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
}

LRESULT ObjectView::AddressWndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RETURN:
			return SendMessage(GetParent(handle), WM_COMMAND, MAKEWPARAM(2, BN_CLICKED), 0);
		}
		break;
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(GetWindowLongPtr(handle, GWLP_USERDATA)), handle, message, wParam, lParam);
}

namespace {
	std::string ReadShitVMString(const char* cstr) {
		std::string result;
		char c;
		if (!ReadShitVMMemory(c, cstr)) {
			result = cstr;
		} else {
			do {
				char c;
				ReadShitVMMemory(c, cstr++);
				result.push_back(c);
			} while (result.back());
			result.erase(result.end() - 1);
		}
		return result;
	}
}

void ObjectView::SearchObject() {
	if (!ShitVMProcess) {
		MessageBox(Handle, "ShitVM Process is dead", Title, MB_OK | MB_ICONERROR);
		return;
	}

	for (std::size_t i = 3; i < Children.size(); ++i) {
		DestroyWindow(Children[i].Handle);
	}
	Children.erase(Children.begin() + 3, Children.end());

	char addressBuffer[33];
	GetWindowText(Children[0].Handle, addressBuffer, sizeof(addressBuffer));

	unsigned long long addressInt = std::stoull(addressBuffer, nullptr, 16);
	const bool isManaged = SendMessage(Children[1].Handle, BM_GETCHECK, 0, 0) == BST_CHECKED;

	std::uint8_t buffer[sizeof(svm::ManagedHeapInfo) + sizeof(svm::Type) + sizeof(svm::TypeInfo)];
	svm::ManagedHeapInfo* info = nullptr;
	svm::Type* type = nullptr;
	svm::TypeInfo* typeInfo = nullptr;

	if (!isManaged) {
		type = reinterpret_cast<svm::Type*>(buffer);
		typeInfo = reinterpret_cast<svm::TypeInfo*>(buffer + sizeof(svm::Type));
		ReadShitVMMemory(*type, addressInt);
		ReadShitVMMemory(*typeInfo, type->GetPointer());
	} else {
		info = reinterpret_cast<svm::ManagedHeapInfo*>(buffer);
		type = reinterpret_cast<svm::Type*>(buffer + sizeof(svm::Type));
		typeInfo = reinterpret_cast<svm::TypeInfo*>(buffer + sizeof(svm::ManagedHeapInfo) + sizeof(svm::Type));
		ReadShitVMMemory(*info, addressInt);
		ReadShitVMMemory(*type, addressInt + sizeof(svm::ManagedHeapInfo));
		ReadShitVMMemory(*typeInfo, type->GetPointer());
	}

	const std::uint32_t code = static_cast<std::uint32_t>(typeInfo->Code);
	std::uint8_t dataBuffer[16];
	if (code < 10) {
		ReadShitVMMemory(dataBuffer, reinterpret_cast<const void*>(addressInt), typeInfo->Size);
	}

#define ARITHMETIC(name, object) m_ObjectType = "Type: " name; m_ObjectValue = "Value: " + std::to_string(reinterpret_cast<svm:: object*>(dataBuffer)->Value)

	switch (code) {
	case 3: ARITHMETIC("int", IntObject); break;
	case 4: ARITHMETIC("long", LongObject); break;
	case 6: ARITHMETIC("double", DoubleObject); break;
	case 7:
	case 8: {
		if (code == 7) {
			m_ObjectType = "Type: pointer";
		} else {
			m_ObjectType = "Type: gcpointer";
		}
		void* const address = reinterpret_cast<svm::PointerObject*>(dataBuffer)->Value;
		std::ostringstream oss;
		oss << "Value: 0x" << address;
		m_ObjectValue = oss.str();

		if (address) {
			AddChild(CreateButton(this, "View", 380, 52, 100, 25, CallbackLambda(code, address) {
				std::ostringstream oss;
				oss << "0x" << address;
				const std::string addressStr = oss.str();
				SetWindowText(window->Children[0].Handle, addressStr.c_str());
				if (code == 7) {
					SendMessage(window->Children[1].Handle, BM_SETCHECK, BST_UNCHECKED, 0);
				} else {
					SendMessage(window->Children[1].Handle, BM_SETCHECK, BST_CHECKED, 0);
				}
				return window->Send(WM_COMMAND, MAKEWPARAM(2, BN_CLICKED), 0);
			}));
		}
		break;
	}
	default: {
		if (code >= 10) {
			m_ObjectType = "Type: " + ReadShitVMString(typeInfo->Name.c_str());
		} else {
			m_ObjectType = "Type: (Unknown)";
		}
		break;
	}
	}

	if (code >= 10) {
		std::ostringstream oss;
		oss << "Fields:\n";

		std::uint8_t* const structBuffer = new std::uint8_t[typeInfo->Size];
		ReadShitVMMemory(structBuffer, reinterpret_cast<const void*>(addressInt + sizeof(svm::ManagedHeapInfo)), typeInfo->Size);

		svm::Type* fieldType = nullptr;
		std::uint8_t fieldTypeInfoBuffer[sizeof(svm::TypeInfo)];
		svm::TypeInfo* fieldTypeInfo = reinterpret_cast<svm::TypeInfo*>(fieldTypeInfoBuffer);

		std::uint32_t i = 0;
		unsigned long long offset = sizeof(svm::Type);

		while (offset < typeInfo->Size) {
			fieldType = reinterpret_cast<svm::Type*>(structBuffer + offset);
			ReadShitVMMemory(*fieldTypeInfo, fieldType->GetPointer());

			oss << "    [" << i++ << "]:\n"
				<< "        Type: " << ReadShitVMString(fieldTypeInfo->Name.c_str()) << '\n'
				<< "        ";

			const std::uint32_t fieldCode = static_cast<std::uint32_t>(fieldTypeInfo->Code);
			switch (fieldCode) {
			case 3: oss << "Value: " << reinterpret_cast<svm::IntObject*>(fieldType)->Value; break;
			case 4: oss << "Value: " << reinterpret_cast<svm::LongObject*>(fieldType)->Value; break;
			case 6: oss << "Value: " << reinterpret_cast<svm::DoubleObject*>(fieldType)->Value; break;
			case 7:
			case 8: {
				void* const address = reinterpret_cast<svm::PointerObject*>(fieldType)->Value;
				oss << "Value: 0x" << address;
				if (address) {
					AddChild(CreateButton(this, "View", 380, 82 + i * 15, 100, 25, CallbackLambda(fieldCode, address) {
						std::ostringstream oss;
						oss << "0x" << address;
						const std::string addressStr = oss.str();
						SetWindowText(window->Children[0].Handle, addressStr.c_str());
						if (fieldCode == 7) {
							SendMessage(window->Children[1].Handle, BM_SETCHECK, BST_UNCHECKED, 0);
						} else {
							SendMessage(window->Children[1].Handle, BM_SETCHECK, BST_CHECKED, 0);
						}
						return window->Send(WM_COMMAND, MAKEWPARAM(2, BN_CLICKED), 0);
					}));
				}
				break;
			}
			default:
				oss << "Address: 0x" << reinterpret_cast<void*>(addressInt + (isManaged ? sizeof(svm::ManagedHeapInfo) : 0) + offset);
				break;
			}

			oss << "\n";
			offset += fieldTypeInfo->Size;
		}

		m_ObjectValue = oss.str();
	}

	Invalidate();
}