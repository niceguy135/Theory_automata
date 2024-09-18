#include "implicant.h"

#include "utils.h"

Implicant::Implicant(const Implicant &other, uint16_t patch) {
    index_ = other.GetIndex();
    value_ = other.GetValue();
    was_patched_ = other.WasPatched();
    patch_ = patch;
}

Implicant::Implicant(uint16_t index, LogicValue value)
    : index_(index)
    , value_(value)
{
    was_patched_ = false;
    patch_ = 0;
}

Implicant::Implicant(const Implicant &other) {
    index_ = other.GetIndex();
    value_ = other.GetValue();
    was_patched_ = other.WasPatched();
    patch_ = other.GetPatch();
}

uint16_t Implicant::GetIndex() const {
    return index_;
}

LogicValue Implicant::GetValue() const {
    return value_;
}

uint16_t Implicant::GetPopcount() const {
    return CountOnes16(GetIndex());
}

uint16_t Implicant::GetPatch() const {
    return patch_; 
}

bool Implicant::WasPatched() const {
    return was_patched_;
}

bool Implicant::CanPatch(const Implicant& other) const {
    return (other.GetIndex() > GetIndex()) &&
            ((other.GetPopcount() - GetPopcount()) == 1) &&
            (GetDiffPopcount(other) == 1) &&
            (GetPatch() == other.GetPatch());
}

uint16_t Implicant::GetDiffPopcount(const Implicant& other) const {
    uint16_t diff = (other.GetIndex() > GetIndex())
                    ? other.GetIndex() - GetIndex()
                    : GetIndex() - other.GetIndex();

    return CountOnes16(diff);
}

bool Implicant::operator==(const Implicant& other) const {
    return index_ == other.index_ &&
            value_ == other.value_ &&
            was_patched_ == other.was_patched_ &&
            patch_ == other.patch_;
}

bool Implicant::operator!=(const Implicant& other) const {
    return !(other == *this);
}

Implicant Implicant::Patch(Implicant &other) {
    if (CanPatch(other)) {
        was_patched_ = true;
        other.SetPatched(true);
        return Implicant(*this, GetPatch() + other.GetIndex() - GetIndex());
    } else {
        throw std::runtime_error("Can't patch. Please, use Implicant::CanPatch/1 before calling Implicant::Patch");
    }
}

void Implicant::SetPatched(bool value) {
    was_patched_ = value;
}

void Implicant::PrintImplicant(uint8_t count) const {
    char value = LogicValueToChar(GetValue());

    for (uint8_t i = 0; i < count; ++i) {
        std::cout << value;
    }
    std::cout << std::endl;
}

std::string Implicant::ImplicantStringRepr() const {
    return (
        std::string("[") +
        std::to_string(GetIndex()) +
        std::string(";") +
        std::to_string(GetPopcount()) +
        std::string(";") +
        std::to_string(GetPatch()) +
        std::string("]")
    );
} 

std::string Implicant::ImplicantToString(uint8_t size) const {
    std::string result("");

    for (uint8_t i = 0; i < size; i++) {
        bool patched = (GetPatch() & (0x1 << i)) >> i;
        uint8_t value = (GetIndex() & (0x1 << i)) >> i;

        if (patched) {
            result.insert(0, "-");
        } else if (value == 1) { // Yes, those are conditionals
            result.insert(0, "1");
        } else { // Yes, it is a *better* design than doing casts and other dirty tricks
            result.insert(0, "0");
        }
    }

    return result;
}

void Implicant::PrintVector(std::vector<Implicant>& vec) {
    std::cout << "N: ";

    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetIndex() << " ";
    });

    std::cout << std::endl;

    std::cout << "I: ";
    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetPopcount() << " ";
    });

    std::cout << std::endl << "P: ";
    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetPatch() << " ";
    });


    std::cout << std::endl;
    std::cout << "W: ";
    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.WasPatched() << " ";
    });

    std::cout << std::endl;
}

void Implicant::PrintSet(std::set<Implicant>& vec) {
    std::cout << "N: ";

    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetIndex() << " ";
    });

    std::cout << std::endl;

    std::cout << "I: ";
    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetPopcount() << " ";
    });

    std::cout << std::endl;
    std::cout << "P: ";
    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetPatch() << " ";
    });


    std::cout << std::endl;
    std::cout << "W: ";
    std::for_each(vec.begin(), vec.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.WasPatched() << " ";
    });

    std::cout << std::endl;
}

void Implicant::PatchVectors(std::vector<Implicant>& source, std::vector<Implicant>& target, bool verbose) {
    for (uint16_t i = 0; i < source.size(); i++) {
        for (uint16_t j = i + 1; j < source.size(); j++) {
            if (source[i].CanPatch(source[j])) {
                target.push_back(source[i].Patch(source[j]));
                if (verbose) {
                    std::cout << "Patching " << source[i].ImplicantStringRepr() << " with " << source[j].ImplicantStringRepr() << std::endl;
                }
            }
        }
    }

    for (uint16_t i = 0; i < source.size(); i++) {
        if (!source[i].WasPatched()) {

            if (verbose) {
                std::cout << "Adding unpached value: " << source[i].ImplicantStringRepr() << std::endl;
            }
            target.push_back(Implicant(source[i]));
        }
    }
}

void Implicant::PrintTable(std::vector<Implicant> func, std::vector<Implicant> dnf, const uint8_t& func_size) {
    std::vector<Implicant> values;

    std::remove_copy_if(func.begin(), func.end(), back_inserter(values), [](const Implicant& x) {
        return x.GetValue() != TRUE;
    });

    std::cout << std::setw(4) << " ";

    std::for_each(values.begin(), values.end(), [](const Implicant& x) {
        std::cout << std::setw(2) << x.GetIndex() << " ";
    });
    std::cout << std::endl;

    std::for_each(dnf.rbegin(), dnf.rend(), [&values, &func_size](const Implicant& x) {
        std::cout << std::setw(4) << x.ImplicantToString(func_size);

        std::for_each(values.begin(), values.end(), [&x](const Implicant& y) {
            std::cout << std::setw(2) << (((y.GetIndex() & ~x.GetPatch()) == x.GetIndex()) ? "+" : "-") << " ";
        });
        std::cout << std::endl;
    });
}
