#include <window/ObjectViewer.hpp>

#include <Application.hpp>
#include <ShitVM.hpp>
#include <svm/GarbageCollector.hpp>
#include <svm/Object.hpp>
#include <svm/Type.hpp>

#include <cstddef>
#include <cstdint>
#include <sstream>

#ifdef _MSC_VER
#	pragma warning(disable: 4100)
#endif

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
		if (reinterpret_cast<HWND>(lParam) == Children[1].Handle) {
			const HDC hdc = reinterpret_cast<HDC>(wParam);
			SetBkColor(hdc, RGB(255, 255, 255));
			return reinterpret_cast<LRESULT>(m_Brush);
		}
		break;
	}

	case WM_DESTROY:
		m_ObjectType = "Type: (Not searched)";
		m_ObjectValue.clear();
		break;
	}
	return Window::Callback(handle, message, wParam, lParam);
}

void ObjectView::Initialize() {
	AddChild(CreateTextBox(this, 80, 12, 200, 25, nullptr))
		.SetWndProc(AddressWndProc);

	AddChild(CreateCheckBox(this, "Managed", 290, 12, 80, 25, nullptr));

	AddChild(CreateButton(this, "Search", 380, 12, 100, 25, CallbackLambda(this) {
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
				ReadShitVMMemory(c, cstr++);
				result.push_back(c);
			} while (result.back());
			result.erase(result.end() - 1);
		}
		return result;
	}
}

void ObjectView::SearchObject() {
	for (std::size_t i = 3; i < Children.size(); ++i) {
		DestroyWindow(Children[i].Handle);
	}
	Children.erase(Children.begin() + 3, Children.end());

	if (!ShitVMProcess) {
		m_ObjectType = "Type: (Not searched)";
		m_ObjectValue.clear();
		Invalidate();
		MessageBox(Handle, "Dead process", "Object Viewer", MB_OK | MB_ICONERROR);
		return;
	}
	
	char addressBuffer[33];
	GetWindowText(Children[0].Handle, addressBuffer, sizeof(addressBuffer));

	const unsigned long long addressInt = std::stoull(addressBuffer, nullptr, 16);
	const bool isManaged = SendMessage(Children[1].Handle, BM_GETCHECK, 0, 0) == BST_CHECKED;

	if (!CheckReadableShitVMMemory(reinterpret_cast<void*>(addressInt), sizeof(svm::Type) + (isManaged ? sizeof(svm::ManagedHeapInfo) : 0))) {
		m_ObjectType = "Type: (Not searched)";
		m_ObjectValue.clear();
		Invalidate();
		MessageBox(Handle, "Unreadable address", "Object Viewer", MB_OK | MB_ICONERROR);
		return;
	}

	std::uint8_t typeBuffer[sizeof(svm::ManagedHeapInfo) + sizeof(svm::Type) + sizeof(svm::TypeInfo)];
	svm::ManagedHeapInfo* const info = reinterpret_cast<svm::ManagedHeapInfo*>(typeBuffer);
	svm::Type* const type = reinterpret_cast<svm::Type*>(typeBuffer + sizeof(svm::ManagedHeapInfo));
	svm::TypeInfo* const typeInfo = reinterpret_cast<svm::TypeInfo*>(typeBuffer + sizeof(svm::ManagedHeapInfo) + sizeof(svm::Type));

	bool success = true;

	if (isManaged) {
		ReadShitVMMemory(*info, addressInt);
		ReadShitVMMemory(*type, addressInt + sizeof(svm::ManagedHeapInfo));
		success = ReadShitVMMemory(*typeInfo, type->GetPointer());
	} else {
		ReadShitVMMemory(*type, addressInt);
		success = ReadShitVMMemory(*typeInfo, type->GetPointer());
	}

	if (!success) {
		m_ObjectType = "Type: (Not searched)";
		m_ObjectValue.clear();
		Invalidate();
		MessageBox(Handle, "Unreadable address", "Object Viewer", MB_OK | MB_ICONERROR);
		return;
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

		void* const target = reinterpret_cast<svm::PointerObject*>(dataBuffer)->Value;
		std::ostringstream oss;
		oss << "Value: 0x" << target;
		m_ObjectValue = oss.str();

		if (target) {
			AddChild(CreateButton(this, "View", 380, 52, 100, 25, CallbackLambda(code, target) {
				std::ostringstream oss;
				oss << "0x" << target;

				SetWindowText(window->Children[0].Handle, oss.str().c_str());
				SendMessage(window->Children[1].Handle, BM_SETCHECK, code == 8 ? BST_CHECKED : BST_UNCHECKED, 0);
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
		ReadShitVMMemory(structBuffer, reinterpret_cast<void*>(addressInt + (isManaged ? sizeof(svm::ManagedHeapInfo) : 0)), typeInfo->Size);

		std::uint8_t fieldTypeBuffer[sizeof(svm::TypeInfo)];
		svm::Type* fieldType = nullptr;
		svm::TypeInfo* const fieldTypeInfo = reinterpret_cast<svm::TypeInfo*>(fieldTypeBuffer);

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
				void* const target = reinterpret_cast<svm::PointerObject*>(fieldType)->Value;
				oss << "Value: 0x" << target;

				if (target) {
					AddChild(CreateButton(this, "View", 380, 82 + i * 15, 100, 25, CallbackLambda(fieldCode, target) {
						std::ostringstream oss;
						oss << "0x" << target;

						SetWindowText(window->Children[0].Handle, oss.str().c_str());
						SendMessage(window->Children[1].Handle, BM_SETCHECK, fieldCode == 8 ? BST_CHECKED : BST_UNCHECKED, 0);
						return window->Send(WM_COMMAND, MAKEWPARAM(2, BN_CLICKED), 0);
					}));
				}

				break;
			}
			default:
				oss << "Address: 0x" << reinterpret_cast<void*>(addressInt + (isManaged ? sizeof(svm::ManagedHeapInfo) : 0) + offset);
				break;
			}

			oss << '\n';
			offset += fieldTypeInfo->Size;
		}

		m_ObjectValue = oss.str();
		delete[] structBuffer;
	}

	Invalidate();
}