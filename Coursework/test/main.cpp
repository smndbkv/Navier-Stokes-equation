#include <iostream>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>

int main()
{
    // Размер системы (можно менять)
    const int N = 100000;

    // 1. Создаём разреженную матрицу (например, трёхдиагональная матрица из 1D задачи Пуассона)
    Eigen::SparseMatrix<double> A(N, N);
    A.reserve(3 * N); // резервируем память для трёх ненулевых элементов в строке

    // Заполнение матрицы: A(i,i) = 2, A(i,i-1) = -1, A(i,i+1) = -1
    for (int i = 0; i < N; ++i)
    {
        A.insert(i, i) = 2.0;
        if (i > 0)
            A.insert(i, i - 1) = -1.0;
        if (i < N - 1)
            A.insert(i, i + 1) = -1.0;
    }
    A.makeCompressed(); // преобразуем в сжатый формат для эффективности

    // 2. Правая часть b (например, вектор единиц)
    Eigen::VectorXd b = Eigen::VectorXd::Ones(N);

    // 3. Создаём решатель BiCGSTAB с предобуславливателем (по умолчанию DiagonalPreconditioner)
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower | Eigen::Upper> solver;

    // Можно выбрать другой предобуславливатель, например IdentityPreconditioner (без предобуславливания)
    // Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> solver;

    // 4. Задаём матрицу и настраиваем параметры (опционально)
    solver.setTolerance(1e-8);  // желаемая точность
    solver.setMaxIterations(N); // максимум итераций
    solver.compute(A);          // подготавливаем матрицу (вычисляем предобуславливатель)

    if (solver.info() != Eigen::Success)
    {
        std::cerr << "Ошибка при разложении матрицы" << std::endl;
        return 1;
    }

    // 5. Решаем систему
    Eigen::VectorXd x = solver.solve(b);

    // 6. Проверяем успешность решения
    if (solver.info() == Eigen::Success)
    {
        std::cout << "Решение найдено за " << solver.iterations() << " итераций.\n";
        std::cout << "Невязка: " << solver.error() << "\n";
        // Выведем первые 10 компонент решения
        std::cout << "x (первые 10): " << x.head(10).transpose() << std::endl;
    }
    else
    {
        std::cerr << "Решение не сошлось" << std::endl;
    }

    // Для проверки вычислим невязку: b - A*x
    Eigen::VectorXd residual = b - A * x;
    std::cout << "Норма невязки: " << residual.norm() << std::endl;

    return 0;
}